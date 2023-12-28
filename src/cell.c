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
    cell->positionInit.x = x;
    cell->positionInit.y = y;

    cell->angleVelocity = 4.0f;

    cell->speedMin = -2.0f;
    cell->speedMax = 3.0f;
    cell->velocity = 0.4f;

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
    int topology[] = {7, 50, 20, 4};
    cell->nn = createNeuralNetwork(topology, 4);
    setRandomWeights(cell->nn, -1, 1);

    return cell;
}

void Cell_update(Cell *cell, Map *map)
{
    if (!cell->isAlive)
        return;

    if (cell->isAI)
    {
        double inputs[] = {
            cell->rays[0].distance / cell->rays[0].distanceMax,
            cell->rays[1].distance / cell->rays[1].distanceMax,
            cell->rays[2].distance / cell->rays[2].distanceMax,
            cell->rays[3].distance / cell->rays[3].distanceMax,
            cell->rays[4].distance / cell->rays[4].distanceMax,
            cell->rays[5].distance / cell->rays[5].distanceMax,
            cell->rays[6].distance / cell->rays[6].distanceMax
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

    // Check cell collision with walls
    for (int i = 0; i < 4; i++)
    {
        if (check_rect_collision(cell, &map->walls[i]->rect))
        {
            cell->isAlive = false;
            break;
        }
    }

    // Get score
    cell->score = (int)cell->position.x - 50;
    if (!cell->isAlive)
        cell->score = 0;

    // Calculate rays
    for (int i = 0; i < 7; i++)
    {
        float distance = cell->rays[i].distanceMax;
        for (int j = 0; j < 4; j++)
        {
            float newDistance = check_ray_collision(cell, &map->walls[j]->rect, i);
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

    if (cell->score < 100) {
        for (int i = 0; i < 10; ++i)
            mutateNeuralNetwork(cell->nn, parent->nn, mutationRate, mutationProbability);
    }

    // Mutate color
    if (Utils_rand(0, 10) < 5)
    {
        cell->color.r += 1;
        cell->color.g += 1;
        cell->color.b += 1;
        if (cell->color.r == 0)
            cell->color.r = 255;
        if (cell->color.g == 0)
            cell->color.g = 255;
        if (cell->color.b == 0)
            cell->color.b = 255;
    }
    else
    {
        cell->color.r -= 1;
        cell->color.g -= 1;
        cell->color.b -= 1;
        if (cell->color.r == 255)
            cell->color.r = 0;
        if (cell->color.g == 255)
            cell->color.g = 0;
        if (cell->color.b == 255)
            cell->color.b = 0;
    }
}

void Cell_render(Cell *cell, SDL_Renderer *renderer)
{
    if (!cell->isAlive)
    {
        // Render dead cell
        SDL_SetRenderDrawColor(renderer,
                                 COLOR_RED.r,
                                 COLOR_RED.g,
                                 COLOR_RED.b,
                                 COLOR_RED.a);

        // Render filled circle
        SDL_RenderFillCircle(renderer, cell->position.x, cell->position.y, cell->radius);
        return;
    }

    // Set renderer color to cell color
    SDL_SetRenderDrawColor(renderer,
                           cell->color.r,
                           cell->color.g,
                           cell->color.b,
                           cell->color.a);

    // Render filled circle
    SDL_RenderFillCircle(renderer, cell->position.x, cell->position.y, cell->radius);

    // if dead, not render smile & rays
    if (!cell->isAlive)
        return;

    // Render rays
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
}

void Cell_reset(Cell *cell)
{
    cell->isAlive = true;
    cell->score = 0;

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
