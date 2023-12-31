#include "cell.h"

void render_healthbar(Cell *cell, SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, COLOR_WHITE.r, COLOR_WHITE.g, COLOR_WHITE.b, COLOR_WHITE.a);
    SDL_RenderDrawRect(renderer, &(SDL_Rect){cell->position.x - cell->radius, cell->position.y - cell->radius - 10, cell->radius * 2, 5});
    SDL_SetRenderDrawColor(renderer, COLOR_GREEN.r, COLOR_GREEN.g, COLOR_GREEN.b, COLOR_GREEN.a);
    SDL_RenderFillRect(renderer, &(SDL_Rect){cell->position.x - cell->radius, cell->position.y - cell->radius - 10, cell->radius * 2 * (float)cell->health / (float)cell->healthMax, 5});
}

void render_face(Cell *cell, SDL_Renderer *renderer)
{
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

void render_rays(Cell *cell, SDL_Renderer *renderer)
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

void Cell_render(Cell *cell, SDL_Renderer *renderer, bool renderRays, bool isSelected)
{
    if (!cell->isAlive)
        return;

    // Render filled circle
    if (isSelected)
    {
        SDL_SetRenderDrawColor(renderer, COLOR_ORANGE.r, COLOR_ORANGE.g, COLOR_ORANGE.b, COLOR_ORANGE.a);
        SDL_RenderFillCircle(renderer, cell->position.x, cell->position.y, cell->radius);
        render_face(cell, renderer);
    }
    else if (cell->sprite != NULL)
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
    else
    {
        SDL_SetRenderDrawColor(renderer, COLOR_VIOLET.r, COLOR_VIOLET.g, COLOR_VIOLET.b, COLOR_VIOLET.a);
        SDL_RenderFillCircle(renderer, cell->position.x, cell->position.y, cell->radius);
        render_face(cell, renderer);
    }

    render_healthbar(cell, renderer);

    if (renderRays)
        render_rays(cell, renderer);
}
