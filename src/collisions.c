#include "collisions.h"


bool check_rect_collision(Cell *cell, SDL_Rect *hitbox)
{
    // Check if the cell is inside the hitbox
    if (cell->position.x + cell->radius > hitbox->x &&
        cell->position.x - cell->radius < hitbox->x + hitbox->w &&
        cell->position.y + cell->radius > hitbox->y &&
        cell->position.y - cell->radius < hitbox->y + hitbox->h)
    {
        // Check if the cell is above the hitbox
        if (cell->position.y + cell->radius > hitbox->y &&
            cell->position.y - cell->radius < hitbox->y)
        {
            cell->position.y = hitbox->y - cell->radius;
        }
        // Check if the cell is below the hitbox
        else if (cell->position.y - cell->radius < hitbox->y + hitbox->h &&
                 cell->position.y + cell->radius > hitbox->y + hitbox->h)
        {
            cell->position.y = hitbox->y + hitbox->h + cell->radius;
        }
        // Check if the cell is on the left of the hitbox
        else if (cell->position.x + cell->radius > hitbox->x &&
                 cell->position.x - cell->radius < hitbox->x)
        {
            cell->position.x = hitbox->x - cell->radius;
        }
        // Check if the cell is on the right of the hitbox
        else if (cell->position.x - cell->radius < hitbox->x + hitbox->w &&
                 cell->position.x + cell->radius > hitbox->x + hitbox->w)
        {
            cell->position.x = hitbox->x + hitbox->w + cell->radius;
        }

        return true;
    }

    return false;
}
