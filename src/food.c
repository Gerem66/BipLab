#include "food.h"


Food *Food_init(float x, float y)
{
    Food *food = malloc(sizeof(Food));
    if (food == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for Food !\n");
        return NULL;
    }

    food->rect.x = x;
    food->rect.y = y;
    food->rect.w = 10;
    food->rect.h = 10;
    food->value = 50;
    food->color = COLOR_YELLOW;

    return food;
}

void Food_reset(Food *food, Map *map)
{
    food->rect.x = Utils_rand(0, map->width);
    food->rect.y = Utils_rand(0, map->height);
    food->value = 20;
}

void Food_render(Food *food, SDL_Renderer *renderer, bool renderTexts)
{
    SDL_SetRenderDrawColor(renderer, COLOR_BREAKUP(food->color));
    SDL_RenderFillRectF(renderer, &food->rect);

    if (!renderTexts)
        return;
    char text[20];
    sprintf(text, "%d", food->value);
    stringRGBA(renderer, food->rect.x, food->rect.y - 10, text,
               food->color.r, food->color.g, food->color.b, food->color.a);
}

void Food_destroy(Food *food)
{
    free(food);
}
