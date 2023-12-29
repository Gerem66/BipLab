#include "cell.h"


Cell *Cell_init(int x, int y, bool isAI)
{
    Cell *cell = malloc(sizeof(Cell));
    if (cell == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for Cell !\n");
        return NULL;
    }

    cell->isAI = isAI;
    cell->maxScore = 100;
    cell->positionInit.x = x;
    cell->positionInit.y = y;
    cell->healthInit = 20;
    cell->healthMax = 50;
    cell->framePerHealth = 30;
    cell->birthCostMax = 5;
    cell->generation = 1;

    cell->speedMin = -2.0f;
    cell->speedMax = 3.0f;
    cell->velocity = 0.4f;
    cell->angleVelocity = 4.0f;

    cell->radius = 10;
    cell->color = COLOR_BLUE;
    if (!isAI)
        cell->color = COLOR_GREEN;

    // Init rays from -PI to PI
    for (int i = 0; i < 7; i++)
    {
        cell->rays[i].angle = -PI + i * PI / 3;
        cell->rays[i].distance = 0.0f;
        cell->rays[i].distanceMax = 100.0f;
    }

    Cell_reset(cell);

    // Create NeuralNetwork
    int topology[] = {8, 20, 10, 4};
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
        double inputs[] = {
            (double)(cell->health / cell->healthMax),
            (double)(cell->rays[0].distance / cell->rays[0].distanceMax),
            (double)(cell->rays[1].distance / cell->rays[1].distanceMax),
            (double)(cell->rays[2].distance / cell->rays[2].distanceMax),
            (double)(cell->rays[3].distance / cell->rays[3].distanceMax),
            (double)(cell->rays[4].distance / cell->rays[4].distanceMax),
            (double)(cell->rays[5].distance / cell->rays[5].distanceMax),
            (double)(cell->rays[6].distance / cell->rays[6].distanceMax)
        };
        double outputs[4];
        processInputs(cell->nn, inputs, outputs);

        cell->goingUp = outputs[0] > 0.5;
        cell->goingDown = outputs[1] > 0.5;
        cell->goingLeft = outputs[2] > 0.5;
        cell->goingRight = outputs[3] > 0.5;
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

    // Check cell collision with foods
    if (cell->frame % 10 == 0)
    {
        for (int i = 0; i < FOOD_COUNT; i++)
        {
            float distance = sqrt(pow(cell->position.x - map->foods[i]->rect.x, 2) + pow(cell->position.y - map->foods[i]->rect.y, 2));
            if (distance < cell->radius + map->foods[i]->rect.w)
            {
                if (cell->health < cell->healthMax)
                {
                    if (cell->score < cell->maxScore)
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

    // Calculate rays
    for (int i = 0; i < 7; i++)
    {
        float distance = cell->rays[i].distanceMax;
        for (int j = 0; j < FOOD_COUNT; j++)
        {
            float newDistance = check_ray_collision(cell, &map->foods[j]->rect, i);
            if (newDistance < distance)
                distance = newDistance;
            if (newDistance < 0.0f)
                distance = 0.0f;
        }
        cell->rays[i].distance = distance;
    }
}

void Cell_mutate(Cell *cell, Cell *parent, float mutationRate, float mutationProbability)
{
    // Mutate NeuralNetwork
    mutateNeuralNetwork(cell->nn, parent->nn, mutationRate, mutationProbability);
}

void Cell_GiveBirth(Cell *cell, Map *map)
{
    int index = -1;
    for (int i = 1; i < CELL_COUNT; i++)
    {
        if (map->cells[i] != NULL && !map->cells[i]->isAlive)
        {
            index = i;
            Cell_destroy(map->cells[i]);
            break;
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

    Cell *newCell = Cell_init(cell->positionInit.x, cell->positionInit.y, true);
    newCell->position.x = cell->position.x;
    newCell->position.y = cell->position.y;
    newCell->generation = cell->generation + 1;
    Cell_mutate(newCell, cell, 1.0f, 0.2f);
    map->cells[index] = newCell;
}

void Cell_render(Cell *cell, SDL_Renderer *renderer, bool renderRays)
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
    SDL_RenderFillCircle(renderer, cell->position.x, cell->position.y, cell->radius);

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

            // Render ray intersection
            SDL_RenderFillCircle(renderer,
                                cell->position.x + cell->rays[i].distance * cos(cell->rays[i].angle + cell->angle * PI / 180.0f),
                                cell->position.y + cell->rays[i].distance * sin(cell->rays[i].angle + cell->angle * PI / 180.0f),
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
    freeNeuralNetwork(cell->nn);
    free(cell);
}
