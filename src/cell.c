#include "cell.h"


Cell *Cell_init(int x, int y, int radius)
{
    Cell *cell = malloc(sizeof(Cell));
    if (cell == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for Cell !\n");
        return NULL;
    }

    cell->positionInit.x = x;
    cell->positionInit.y = y;

    cell->angleVelocity = 4.0f;

    cell->speedMin = -3.0f;
    cell->speedMax = 6.0f;
    cell->velocity = 0.5f;

    cell->radius = radius;
    cell->color = COLOR_BLUE;

    Cell_reset(cell);

    return cell;
}

void Cell_update(Cell *cell, Map *map)
{
    if (!cell->isAlive)
        return;

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
    cell->position.x += cell->speed * (float)cos(cell->angle * PI / 180);
    cell->position.y += cell->speed * (float)sin(cell->angle * PI / 180);

    // Get score
    cell->score = (int)cell->position.x - 50;

    // Check cell collision with walls
    for (int i = 0; i < 4; i++)
    {
        if (check_rect_collision(cell, &map->walls[i]->rect))
        {
            cell->isAlive = false;
            break;
        }
    }
}

void Cell_render(Cell *cell, SDL_Renderer *renderer)
{
    // Set renderer color to cell color
    SDL_SetRenderDrawColor(renderer,
                           cell->color.r,
                           cell->color.g,
                           cell->color.b,
                           cell->color.a);

    if (!cell->isAlive)
    {
        // Render dead cell
        SDL_SetRenderDrawColor(renderer,
                                 COLOR_RED.r,
                                 COLOR_RED.g,
                                 COLOR_RED.b,
                                 COLOR_RED.a);
        return;
    }

    // Render filled circle
    SDL_RenderFillCircle(renderer, cell->position.x, cell->position.y, cell->radius);

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
    free(cell);
}
