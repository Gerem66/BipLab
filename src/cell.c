#include "cell.h"


Cell *Cell_init(SDL_Texture *sprite, int x, int y, bool isAI)
{
    Cell *cell = malloc(sizeof(Cell));
    if (cell == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for Cell !\n");
        return NULL;
    }

    cell->isAI = isAI;
    cell->positionInit.x = x;
    cell->positionInit.y = y;
    cell->healthInit = 20;
    cell->healthMax = 50;
    cell->framePerHealth = 30;
    cell->birthCostMax = 15;

    cell->speedMin = -2.0f;
    cell->speedMax = 3.0f;
    cell->velocity = 0.4f;
    cell->angleVelocity = 4.0f;

    cell->radius = 10;
    cell->color = COLOR_BLUE;
    if (!isAI)
        cell->color = COLOR_GREEN;
    cell->sprite = sprite;

    // Init rays from -PI to PI
    for (int i = 0; i < 7; i++)
    {
        cell->rays[i].angle = -PI + i * PI / 3;
        cell->rays[i].distance = 0.0f;
        cell->rays[i].distanceMax = 100.0f;
        cell->raysWall[i].angle = -PI + i * PI / 3;
        cell->raysWall[i].distance = 0.0f;
        cell->raysWall[i].distanceMax = 100.0f;
    }

    Cell_reset(cell);

    // Create NeuralNetwork
    int topology[] = {15, 6, 6, 4};
    cell->nn = createNeuralNetwork(topology, 4);
    setRandomWeights(cell->nn, -1, 1);

    return cell;
}

void Cell_update(Cell *cell, Map *map)
{
    if (!cell->isAlive)
        return;

    // Update health
    cell->frame++;
    if (cell->frame % cell->framePerHealth == 0)
    {
        cell->health--;
        if (cell->health <= 0)
            cell->isAlive = false;
    }

    if (cell->isAI)
    {
        cell->inputs[0] = (double)cell->health / (double)cell->healthMax,
        cell->inputs[1] = (double)(1 - cell->rays[0].distance / cell->rays[0].distanceMax);
        cell->inputs[2] = (double)(1 - cell->rays[1].distance / cell->rays[1].distanceMax);
        cell->inputs[3] = (double)(1 - cell->rays[2].distance / cell->rays[2].distanceMax);
        cell->inputs[4] = (double)(1 - cell->rays[3].distance / cell->rays[3].distanceMax);
        cell->inputs[5] = (double)(1 - cell->rays[4].distance / cell->rays[4].distanceMax);
        cell->inputs[6] = (double)(1 - cell->rays[5].distance / cell->rays[5].distanceMax);
        cell->inputs[7] = (double)(1 - cell->rays[6].distance / cell->rays[6].distanceMax);
        cell->inputs[8] = (double)(1 - cell->raysWall[0].distance / cell->raysWall[0].distanceMax);
        cell->inputs[9] = (double)(1 - cell->raysWall[1].distance / cell->raysWall[1].distanceMax);
        cell->inputs[10] = (double)(1 - cell->raysWall[2].distance / cell->raysWall[2].distanceMax);
        cell->inputs[11] = (double)(1 - cell->raysWall[3].distance / cell->raysWall[3].distanceMax);
        cell->inputs[12] = (double)(1 - cell->raysWall[4].distance / cell->raysWall[4].distanceMax);
        cell->inputs[13] = (double)(1 - cell->raysWall[5].distance / cell->raysWall[5].distanceMax);
        cell->inputs[14] = (double)(1 - cell->raysWall[6].distance / cell->raysWall[6].distanceMax);
        processInputs(cell->nn, cell->inputs, cell->outputs);

        cell->goingUp = cell->outputs[0] > 0.5;
        cell->goingDown = cell->outputs[1] > 0.5;
        cell->goingLeft = cell->outputs[2] > 0.5;
        cell->goingRight = cell->outputs[3] > 0.5;
    }

    // Update angle
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

    // Update speed
    if (cell->goingUp)
    {
        cell->speed += cell->velocity;
        cell->speed = MIN(cell->speed, cell->speedMax);
    }
    else if (cell->goingDown)
    {
        cell->speed -= cell->velocity;
        cell->speed = MAX(cell->speed, cell->speedMin);
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

    // Update position
    cell->position.x += cell->speed * (float)cos(cell->angle * PI / 180.0f);
    cell->position.y += cell->speed * (float)sin(cell->angle * PI / 180.0f);

    // Cell out of bounds
    if (cell->position.x < 0.0f)
        cell->position.x = map->width;
    else if (cell->position.x > map->width)
        cell->position.x = 0.0f;
    if (cell->position.y < 0.0f)
        cell->position.y = map->height;
    else if (cell->position.y > map->height)
        cell->position.y = 0.0f;

    // Check cell collision with walls
    for (int i = 0; i < WALL_COUNT; i++)
    {
        if (SDL_HasIntersection(
            &(SDL_Rect) {
                (int)cell->position.x - cell->radius,
                (int)cell->position.y - cell->radius,
                cell->radius * 2,
                cell->radius * 2
            },
            &(SDL_Rect) {
                map->walls[i]->rect.x,
                map->walls[i]->rect.y,
                map->walls[i]->rect.w,
                map->walls[i]->rect.h
            }))
        {
            cell->isAlive = false;
        }
    }

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
                    cell->score++;
                    cell->health++;
                    map->foods[i]->value--;
                    if (map->foods[i]->value <= 0)
                    {
                        map->foods[i]->value = 20;
                        map->foods[i]->rect.x = rand() % (map->width - 100) + 50;
                        map->foods[i]->rect.y = rand() % (map->height - 100) + 50;
                    }

                    // New cell
                    if (cell->score % cell->birthCostMax == 0)
                        Cell_GiveBirth(cell, map);
                }
            }
        }
    }

    // Calculate food rays
    for (int i = 0; i < 7; i++)
    {
        float distance = cell->rays[i].distanceMax;
        for (int j = 0; j < GAME_START_FOOD_COUNT; j++)
        {
            float newDistance = check_ray_collision(cell, &map->foods[j]->rect, i);
            if (newDistance < distance)
                distance = newDistance;
            if (newDistance < 0.0f)
                distance = 0.0f;
        }
        cell->rays[i].distance = distance;
    }

    // Calculate wall rays
    for (int i = 0; i < 7; i++)
    {
        float distance = cell->raysWall[i].distanceMax;
        for (int j = 0; j < WALL_COUNT; j++)
        {
            // Food rays are used to detect walls because they are the same
            float newDistance = check_ray_collision(cell, &map->walls[j]->rect, i);
            if (newDistance < distance)
                distance = newDistance;
            if (newDistance < 0.0f)
                distance = 0.0f;
        }
        cell->raysWall[i].distance = distance;
    }
}

void Cell_mutate(Cell *cell, Cell *parent, float mutationRate, float mutationProbability)
{
    // Mutate NeuralNetwork
    mutateNeuralNetwork(cell->nn, parent->nn, mutationRate, mutationProbability);
}

void Cell_GiveBirth(Cell *cell, Map *map)
{
    // Priorize lower scores
    int index = -1;
    int minValue =-1;
    for (int i = 1; i < CELL_COUNT; i++)
    {
        if (map->cells[i] == NULL || map->cells[i]->isAlive)
            continue;

        if (map->cells[i]->score < minValue || minValue == -1)
        {
            index = i;
            minValue = map->cells[i]->score;
        }
    }

    if (index == -1 && map->cellCount < CELL_COUNT)
    {
        index = map->cellCount++;
    }

    if (index == -1)
    {
        printf("No more space for new cells !\n");
        return;
    }

    SDL_Texture *sprite = LoadSprite(map->renderer, "../ressources/cell.png");
    if (sprite == NULL)
    {
        printf("Erreur de création de la texture : %s", SDL_GetError());
        return;
    }

    Cell *newCell = Cell_init(sprite, cell->positionInit.x, cell->positionInit.y, true);
    newCell->position.x = cell->position.x;
    newCell->position.y = cell->position.y;
    newCell->generation = cell->generation + 1;
    Cell_mutate(newCell, cell, 1.0f, 0.05f);

    if (map->cells[index] != NULL)
        Cell_destroy(map->cells[index]);
    map->cells[index] = newCell;
}

void Cell_render(Cell *cell, SDL_Renderer *renderer, bool renderRays, bool isSelected)
{
    if (!cell->isAlive)
        return;

    // Set renderer color to cell color
    SDL_SetRenderDrawColor(renderer,
                           255 * (1 - (float)cell->health / (float)cell->healthInit),
                           cell->color.g,
                           255 * ((float)cell->health / (float)cell->healthInit),
                           cell->color.a);

    // Render filled circle
    if (isSelected)
    {
        SDL_SetRenderDrawColor(renderer, COLOR_ORANGE.r, COLOR_ORANGE.g, COLOR_ORANGE.b, COLOR_ORANGE.a);
        SDL_RenderFillCircle(renderer, cell->position.x, cell->position.y, cell->radius);
    }
    else
    {
        // Draw sprite
        int radius = cell->radius * 1.5;
        int angle = cell->angle + 90;
        SDL_Rect positionFond = {
            cell->position.x - radius,
            cell->position.y - radius,
            radius * 2,
            radius * 2
        };
        SDL_RenderCopyEx(renderer, cell->sprite, NULL, &positionFond, angle, NULL, SDL_FLIP_NONE);
    }

    // Render rays
    if (renderRays)
    {
        for (int i = 0; i < 7; i++)
        {
            // Set renderer color to ray color
            if (cell->rays[i].distance < cell->rays[i].distanceMax)
            {
                SDL_SetRenderDrawColor(renderer,
                                    COLOR_RED.r,
                                    COLOR_RED.g,
                                    COLOR_RED.b,
                                    COLOR_RED.a);
            }
            else
            {
                SDL_SetRenderDrawColor(renderer,
                                    COLOR_WHITE.r,
                                    COLOR_WHITE.g,
                                    COLOR_WHITE.b,
                                    COLOR_WHITE.a);
            }

            // Render ray
            SDL_RenderDrawLine(renderer,
                            cell->position.x,
                            cell->position.y,
                            cell->position.x + cell->rays[i].distanceMax * cos(cell->rays[i].angle + cell->angle * PI / 180.0f),
                            cell->position.y + cell->rays[i].distanceMax * sin(cell->rays[i].angle + cell->angle * PI / 180.0f));

            // Render food ray intersection
            SDL_RenderFillCircle(renderer,
                                cell->position.x + cell->rays[i].distance * cos(cell->rays[i].angle + cell->angle * PI / 180.0f),
                                cell->position.y + cell->rays[i].distance * sin(cell->rays[i].angle + cell->angle * PI / 180.0f),
                                2);

            // Render wall ray intersection
            SDL_RenderFillCircle(renderer,
                                cell->position.x + cell->raysWall[i].distance * cos(cell->raysWall[i].angle + cell->angle * PI / 180.0f),
                                cell->position.y + cell->raysWall[i].distance * sin(cell->raysWall[i].angle + cell->angle * PI / 180.0f),
                                2);
        }
    }

    // Draw a smiley face with the cell angle
    // Two small eyes with small arc for the mouth
    int eyeRadius = cell->radius / 8;
    int eyeOffsetX = cell->radius / 1;
    int eyeOffsetY = cell->radius / 2;
    int mouthRadius = cell->radius / 3;
    int mouthOffsetX = cell->radius / 1.5;
    int mouthOffsetY = 0;

    // Rotate eyes around mouth
    int eyeX1 = cell->position.x + eyeOffsetX * cos(cell->angle * PI / 180) - eyeOffsetY * sin(cell->angle * PI / 180);
    int eyeY1 = cell->position.y + eyeOffsetX * sin(cell->angle * PI / 180) + eyeOffsetY * cos(cell->angle * PI / 180);
    int eyeX2 = cell->position.x + eyeOffsetX * cos(cell->angle * PI / 180) + eyeOffsetY * sin(cell->angle * PI / 180);
    int eyeY2 = cell->position.y + eyeOffsetX * sin(cell->angle * PI / 180) - eyeOffsetY * cos(cell->angle * PI / 180);

    int mouthX = cell->position.x + mouthOffsetX * cos(cell->angle * PI / 180) - mouthOffsetY * sin(cell->angle * PI / 180);
    int mouthY = cell->position.y + mouthOffsetX * sin(cell->angle * PI / 180) + mouthOffsetY * cos(cell->angle * PI / 180);

    int mouthStartAngle = 180 + cell->angle - 90;
    int mouthEndAngle = 180 + cell->angle + 90;

    // Draw color
    SDL_SetRenderDrawColor(renderer, COLOR_WHITE.r, COLOR_WHITE.g, COLOR_WHITE.b, COLOR_WHITE.a);

    // Draw eyes
    SDL_RenderFillCircle(renderer, eyeX1, eyeY1, eyeRadius);
    SDL_RenderFillCircle(renderer, eyeX2, eyeY2, eyeRadius);

    // Draw mouth
    SDL_RenderDrawArc(renderer, mouthX, mouthY, mouthRadius, mouthStartAngle, mouthEndAngle);

    // Draw health bar
    SDL_SetRenderDrawColor(renderer, COLOR_WHITE.r, COLOR_WHITE.g, COLOR_WHITE.b, COLOR_WHITE.a);
    SDL_RenderDrawRect(renderer, &(SDL_Rect){cell->position.x - cell->radius, cell->position.y - cell->radius - 10, cell->radius * 2, 5});
    SDL_SetRenderDrawColor(renderer, COLOR_GREEN.r, COLOR_GREEN.g, COLOR_GREEN.b, COLOR_GREEN.a);
    SDL_RenderFillRect(renderer, &(SDL_Rect){cell->position.x - cell->radius, cell->position.y - cell->radius - 10, cell->radius * 2 * (float)cell->health / (float)cell->healthMax, 5});
}

void Cell_reset(Cell *cell)
{
    cell->isAlive = true;
    cell->score = 0;
    cell->health = cell->healthInit;
    cell->frame = 0;
    cell->generation = 1;

    cell->position.x = cell->positionInit.x;
    cell->position.y = cell->positionInit.y;
    cell->speed = 0.0f;
    cell->angle = 0.0f;

    cell->goingUp = false;
    cell->goingDown = false;
    cell->goingLeft = false;
    cell->goingRight = false;
}

void Cell_destroy(Cell *cell)
{
    freeSprite(cell->sprite);
    freeNeuralNetwork(cell->nn);
    free(cell);
}
