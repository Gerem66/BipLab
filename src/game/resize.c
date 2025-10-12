#include "../../include/game.h"

void Game_ResizeWindow(Map *map, int width, int height)
{
    if (map->window) {
        SDL_SetWindowSize(map->window, width, height);

        // Update map dimensions
        map->width = width;
        map->height = height;

        // Center the window
        SDL_SetWindowPosition(map->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
}
