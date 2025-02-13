#include "cell.h"

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

    // Update inputs
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

    // Update outputs
    if (cell->isAI)
    {
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
    cell->hitbox.x = cell->position.x - cell->radius;
    cell->hitbox.y = cell->position.y - cell->radius;

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
    // for (int i = 0; i < 7; i++)
    // {
    //     float distance = cell->raysWall[i].distanceMax;
    //     for (int j = 0; j < GAME_START_WALL_COUNT; j++)
    //     {
    //         // Food rays are used to detect walls because they are the same
    //         float newDistance = check_ray_collision(cell, &map->walls[j]->rect, i);
    //         if (newDistance < distance)
    //             distance = newDistance;
    //         if (newDistance < 0.0f)
    //             distance = 0.0f;
    //     }
    //     cell->raysWall[i].distance = distance;
    // }

    // Calculate other cells rays
    for (int i = 0; i < map->cellCount; i++)
    {
        if (map->cells[i] == NULL || map->cells[i] == cell || !map->cells[i]->isAlive)
            continue;

        for (int j = 0; j < 7; j++)
        {
            float distance = cell->raysWall[i].distanceMax;
            float newDistance = check_ray_collision(cell, &map->cells[i]->hitbox, j);
            if (newDistance < distance)
                distance = newDistance;
            if (newDistance < 0.0f)
                distance = 0.0f;
            cell->raysWall[j].distance = distance;
        }
    }
}

void Cell_mutate(Cell *cell, float mutationRate, float mutationProbability)
{
    mutate_NeuralNetwork_Weights(cell->nn, mutationRate, mutationProbability);
    mutate_NeuralNetwork_Topology(cell->nn, 20, 10, mutationProbability);
}
