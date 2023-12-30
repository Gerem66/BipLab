#include "wall.h"


Wall *Wall_init(float x, float y, float width, float height)
{
    Wall *wall = malloc(sizeof(Wall));
    if (wall == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for Wall !\n");
        return NULL;
    }

    wall->rect.x = x;
    wall->rect.y = y;
    wall->rect.w = width;
    wall->rect.h = height;
    wall->color = COLOR_GRAY;

    return wall;
}

void Wall_reset(Wall *wall, Map *map)
{
    wall->rect.x = Utils_rand(0, map->width - wall->rect.w);
    wall->rect.y = Utils_rand(0, map->height - wall->rect.h);
}

void Wall_render(Wall *wall, SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, COLOR_BREAKUP(wall->color));
    SDL_RenderFillRectF(renderer, &wall->rect);
}

void Wall_destroy(Wall *wall)
{
    free(wall);
}
