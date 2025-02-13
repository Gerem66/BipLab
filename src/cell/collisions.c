#include "cell.h"

// Thanks to ChatGPT

bool check_rect_collision(Cell *cell, SDL_FRect *hitbox)
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

bool findIntersection(SDL_FPoint lineA1, SDL_FPoint lineA2, 
                      SDL_FPoint lineB1, SDL_FPoint lineB2, 
                      SDL_FPoint *intersection) {
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = lineA2.x - lineA1.x;
    s1_y = lineA2.y - lineA1.y;
    s2_x = lineB2.x - lineB1.x;
    s2_y = lineB2.y - lineB1.y;

    float s, t;
    s = (-s1_y * (lineA1.x - lineB1.x) + s1_x * (lineA1.y - lineB1.y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (lineA1.y - lineB1.y) - s2_y * (lineA1.x - lineB1.x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
        // Collision detected
        if (intersection != NULL) {
            intersection->x = lineA1.x + (t * s1_x);
            intersection->y = lineA1.y + (t * s1_y);
        }
        return true;
    }

    return false; // No collision
}

// Fonction pour calculer la distance entre deux points
float distance(SDL_FPoint a, SDL_FPoint b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

// Fonction principale pour trouver la distance d'intersection
float findIntersectionDistance(SDL_FPoint lineA1, SDL_FPoint lineA2, 
                               SDL_FPoint lineB1, SDL_FPoint lineB2) {
    SDL_FPoint intersection;
    if (findIntersection(lineA1, lineA2, lineB1, lineB2, &intersection)) {
        return distance(lineA1, intersection);
    } else {
        return -1; // Pas d'intersection
    }
}

float check_ray_collision(Cell *cell, SDL_FRect *hitbox, int rayIndex)
{
    Ray *ray = &(cell->rays[rayIndex]);

    SDL_FPoint corners[4] = {
        {hitbox->x, hitbox->y},
        {hitbox->x + hitbox->w, hitbox->y},
        {hitbox->x + hitbox->w, hitbox->y + hitbox->h},
        {hitbox->x, hitbox->y + hitbox->h}
    };

    float angle = (cell->angle * PI / 180.0f) + ray->angle;
    float nearestDistance = ray->distanceMax;

    for (int i = 0; i < 4; i++) {
        float newDistance = findIntersectionDistance(cell->position, 
                                                     (SDL_FPoint){cell->position.x + cos(angle) * ray->distanceMax, 
                                                                  cell->position.y + sin(angle) * ray->distanceMax}, 
                                                     corners[i], corners[(i + 1) % 4]);
        if (newDistance != -1 && newDistance < nearestDistance) {
            nearestDistance = newDistance;
        }
    }

    return nearestDistance;
}
