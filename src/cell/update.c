#include "cell.h"

// === Helper functions for input encoding ===
enum HitKind { HK_EMPTY=0, HK_FOOD=1, HK_AGENT=2, HK_WALL=3 };

static inline void set_onehot(double* out4, int k){
    out4[0] = out4[1] = out4[2] = out4[3] = 0.0;
    if (k >= 0 && k < 4) out4[k] = 1.0;
}

// Normalize "value" field depending on hit kind:
// - FOOD:   value = raw food amount -> normalize by FOOD_ITEM_CAPACITY
// - AGENT:  value = raw health -> normalize by agentMaxEnergy
// - others: 0
static inline double norm_value_for(const RayHit* h, double agentMaxEnergy, double foodMaxValue){
    switch ((int)h->type){
        case HK_FOOD:  return CLAMP01(h->value / foodMaxValue);
        case HK_AGENT: return CLAMP01(h->value / agentMaxEnergy);
        default:       return 0.0;
    }
}

void Cell_update(Cell *cell, Map *map)
{
    if (!cell->isAlive)
        return;

    // Update health
    cell->frame++;
    if (cell->frame % CELL_HEALTH_DECAY_FRAMES == 0)
    {
        cell->health--;
        if (cell->health <= 0)
            cell->isAlive = false;
    }

    // === Input encoding: health + reproduction_possible + 7 rays with 6 features each ===
    // Ray features: [distance_norm, state_norm, onehot_empty, onehot_food, onehot_agent, onehot_wall]

    // Input 0: normalized health
    cell->inputs[0] = CLAMP01((double)cell->health / (double)cell->healthMax);

    // Input 1: reproduction possible (1.0 if health > min_health, 0.0 otherwise)
    cell->inputs[1] = (cell->health > CELL_BIRTH_MIN_HEALTH) ? 1.0 : 0.0;

    int idx = 2; // start after health and reproduction_possible
    for (int i = 0; i < NUM_RAYS; ++i){
        const Ray* r = &cell->rays[i];

        bool hasHit = (r->hit.distance >= 0.0) && (r->hit.distance < r->distanceMax);

        int kind = hasHit ? (int)r->hit.type : HK_EMPTY;
        if (kind < 0 || kind > HK_WALL) kind = HK_EMPTY;

        // Distance normalized (1.0 if no hit)
        double dist_norm = hasHit ? CLAMP01(r->hit.distance / (r->distanceMax > 0.0 ? r->distanceMax : 1.0)) : 1.0;
        cell->inputs[idx++] = dist_norm;

        // State value normalized by object type
        double state_norm = hasHit ? norm_value_for(&r->hit, (double)cell->healthMax, (double)FOOD_ITEM_CAPACITY) : 0.0;
        cell->inputs[idx++] = state_norm;

        // One-hot encoding for object type
        set_onehot(&cell->inputs[idx], kind);
        idx += 4;
    }
    // Total inputs: 1 health + 1 reproduction_possible + NUM_RAYS * FEAT_PER_RAY = 44

    // Process neural network
    if (cell->isAI)
    {
        processInputs(cell->nn, cell->inputs, cell->outputs);

        // Update angle from neural output
        cell->angle += cell->outputs[1] * cell->angleVelocity;
        if (cell->angle < 0.0f)
            cell->angle += 360.0f;
        else if (cell->angle >= 360.0f)
            cell->angle -= 360.0f;

        // Calculate target speed from neural output (-1 to 1)
        float targetSpeed = cell->outputs[0] * cell->speedMax;
        if (cell->outputs[0] < 0)
            targetSpeed /= 2;

        float speedDiff = targetSpeed - cell->speed;
        float maxSpeedChange = cell->velocity;

        if (fabs(speedDiff) > maxSpeedChange)
        {
            cell->speed += (speedDiff > 0) ? maxSpeedChange : -maxSpeedChange;
        }
        else
        {
            cell->speed = targetSpeed;
        }

        // Clamp speed within bounds
        cell->speed = MAX(cell->speed, -cell->speedMax / 2);
        cell->speed = MIN(cell->speed, cell->speedMax);

        // Check for reproduction output (outputs[2])
        if (cell->outputs[2] > 0.5)
        {
            if (cell->health > CELL_BIRTH_MIN_HEALTH)
            {
                // Successful reproduction
                // Sacrifice health for reproduction
                cell->health -= CELL_BIRTH_HEALTH_SACRIFICE;

                // Give score bonus for successful reproduction attempt
                cell->score += CELL_BIRTH_SCORE_BONUS * cell->score;

                // Create new cell
                Cell_GiveBirth(cell, map);
            }
            else
            {
                // Failed reproduction attempt - apply penalty
                cell->health -= CELL_BIRTH_FAILED_PENALTY;

                // Ensure cell doesn't die from penalty if it was close to 0
                if (cell->health < 1)
                    cell->health = 1;
            }
        }
    }

    // Manual control mode
    else
    {
        // Rotation
        if (cell->goingLeft)
        {
            cell->angle -= cell->angleVelocity;
            if (cell->angle < 0.0f)
                cell->angle += 360.0f;
        }
        else if (cell->goingRight)
        {
            cell->angle += cell->angleVelocity;
            if (cell->angle > 360.0f)
                cell->angle -= 360.0f;
        }

        // Speed control
        if (cell->goingUp)
        {
            cell->speed += cell->velocity;
            cell->speed = MIN(cell->speed, cell->speedMax);
        }
        else if (cell->goingDown)
        {
            cell->speed -= cell->velocity;
            cell->speed = MAX(cell->speed, -cell->speedMax / 2);
        }
        else if (cell->speed > 0.0f)
        {
            cell->speed -= cell->velocity;
            cell->speed = MAX(cell->speed, 0.0f);
        }
        else if (cell->speed < 0.0f)
        {
            cell->speed += cell->velocity;
            cell->speed = MIN(cell->speed, 0.0f);
        }
    }

    // Update position
    cell->position.x += cell->speed * (float)cos(cell->angle * PI / 180.0f);
    cell->position.y += cell->speed * (float)sin(cell->angle * PI / 180.0f);
    cell->hitbox.x = cell->position.x - cell->radius;
    cell->hitbox.y = cell->position.y - cell->radius;

    // Handle world boundaries (wrap around)
    if (cell->position.x < 0.0f)
        cell->position.x = map->width;
    else if (cell->position.x > map->width)
        cell->position.x = 0.0f;
    if (cell->position.y < 0.0f)
        cell->position.y = map->height;
    else if (cell->position.y > map->height)
        cell->position.y = 0.0f;

    // Check cell collision with walls
    // for (int i = 0; i < GAME_START_WALL_COUNT; i++)
    // {
    //     if (SDL_HasIntersection(
    //         &(SDL_Rect) {
    //             (int)cell->position.x - cell->radius,
    //             (int)cell->position.y - cell->radius,
    //             cell->radius * 2,
    //             cell->radius * 2
    //         },
    //         &(SDL_Rect) {
    //             map->walls[i]->rect.x,
    //             map->walls[i]->rect.y,
    //             map->walls[i]->rect.w,
    //             map->walls[i]->rect.h
    //         }))
    //     {
    //         cell->isAlive = false;
    //     }
    // }

    // Check cell collision with foods
    if (cell->frame % 10 == 0)
    {
        for (int i = 0; i < GAME_START_FOOD_COUNT; i++)
        {
            float distance = sqrt(pow(cell->position.x - map->foods[i]->rect.x, 2) + pow(cell->position.y - map->foods[i]->rect.y, 2));
            if (distance < cell->radius + map->foods[i]->rect.w)
            {
                if (cell->health < cell->healthMax)
                {
                    cell->score += 5;
                    cell->health++;
                    map->foods[i]->value--;

                    if (map->foods[i]->value <= 0)
                    {
                        map->foods[i]->value = FOOD_ITEM_CAPACITY;
                        map->foods[i]->rect.x = rand() % (map->width - 100) + 50;
                        map->foods[i]->rect.y = rand() % (map->height - 100) + 50;
                    }
                }
            }
        }
    }

    // Ray casting for object detection
    for (int i = 0; i < 7; i++)
    {
        float closestDistance = cell->rays[i].distanceMax;
        RayObjectType closestType = RAY_OBJECT_NONE;
        float closestValue = 0.0f;

        // Check food collisions
        for (int j = 0; j < GAME_START_FOOD_COUNT; j++)
        {
            float distance = check_ray_collision(cell, &map->foods[j]->rect, i);
            if (distance >= 0.0f && distance < closestDistance)
            {
                closestDistance = distance;
                closestType = RAY_OBJECT_FOOD;
                closestValue = (float)map->foods[j]->value;
            }
        }

        // Check other cells
        for (int j = 0; j < map->cellCount; j++)
        {
            if (map->cells[j] == NULL || map->cells[j] == cell || !map->cells[j]->isAlive)
                continue;

            float distance = check_ray_collision(cell, &map->cells[j]->hitbox, i);
            if (distance >= 0.0f && distance < closestDistance)
            {
                closestDistance = distance;
                closestType = RAY_OBJECT_CELL;
                closestValue = (float)map->cells[j]->health;
            }
        }

        // Update ray information
        cell->rays[i].distance = closestDistance;
        cell->rays[i].hit.type = closestType;
        cell->rays[i].hit.distance = closestDistance;
        cell->rays[i].hit.value = closestValue;
    }
}

void Cell_mutate(Cell *cell, float mutationRate, float mutationProbability)
{
    mutate_NeuralNetwork_Weights(cell->nn, mutationRate, mutationProbability);
}
