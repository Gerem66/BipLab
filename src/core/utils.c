#include "../../include/core/utils.h"

#include <time.h>


const SDL_Color COLOR_TRANSPARENT   = {0};
const SDL_Color COLOR_BLACK         = {0, 0, 0, 255};
const SDL_Color COLOR_WHITE         = {255, 255, 255, 255};
const SDL_Color COLOR_GRAY          = {100, 100, 100, 255};
const SDL_Color COLOR_DARK_GRAY     = {30, 30, 30, 255};
const SDL_Color COLOR_DARK_GREEN     = {5, 51, 41, 255};
const SDL_Color COLOR_LIGHT_GRAY    = {200, 200, 200, 255};
const SDL_Color COLOR_RED           = {255, 0, 0, 255};
const SDL_Color COLOR_GREEN         = {0, 255, 0, 255};
const SDL_Color COLOR_BLUE          = {0, 0, 255, 255};
const SDL_Color COLOR_YELLOW        = {255, 255, 0, 255};
const SDL_Color COLOR_ORANGE        = {255, 133, 0, 255};
const SDL_Color COLOR_PINK          = {255, 0, 206, 255};
const SDL_Color COLOR_VIOLET        = {145, 0, 255, 255};

void Utils_setBackgroundColor(SDL_Renderer *renderer, SDL_Color color)
{
    // Initialize renderer color
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // Clear screen
    SDL_RenderClear(renderer);
}

void Utils_randInit(void)
{
    srand(time(NULL));
}

float Utils_map(float value, float min1, float max1, float min2, float max2)
{
	return (value - min1) / (max1 - min1) * (max2 - min2) + min2;
}

int irand(int min, int max)
{
    return (rand() % (max + 1)) + min;
}

double drand(double min, double max)
{
    return min + (double)rand() / ((double)RAND_MAX / (max - min));
}
