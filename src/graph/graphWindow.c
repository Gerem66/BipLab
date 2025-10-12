#include "../../include/graph/graphWindow.h"
#include "../../include/graph/graph.h"

#include <stdio.h>

bool GraphWindow_Create(Map *map) {
    if (map->graphWindowOpen) {
        return true; // Already open
    }

    map->graphWindow = SDL_CreateWindow(
        "Evolution Graph - BipLab",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        600,
        250,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    if (map->graphWindow == NULL) {
        fprintf(stderr, "Could not create graph window: %s\n", SDL_GetError());
        return false;
    }

    map->graphRenderer = SDL_CreateRenderer(map->graphWindow, -1, SDL_RENDERER_ACCELERATED);
    if (map->graphRenderer == NULL) {
        fprintf(stderr, "Could not create graph renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(map->graphWindow);
        map->graphWindow = NULL;
        return false;
    }

    map->graphWindowOpen = true;
    return true;
}

void GraphWindow_Destroy(Map *map) {
    if (!map->graphWindowOpen) {
        return;
    }

    if (map->graphRenderer != NULL) {
        SDL_DestroyRenderer(map->graphRenderer);
        map->graphRenderer = NULL;
    }

    if (map->graphWindow != NULL) {
        SDL_DestroyWindow(map->graphWindow);
        map->graphWindow = NULL;
    }

    map->graphWindowOpen = false;
}

void GraphWindow_Render(Map *map) {
    if (!map->graphWindowOpen || map->graphRenderer == NULL) {
        return;
    }

    // Clear the graph window
    SDL_SetRenderDrawColor(map->graphRenderer, 20, 20, 30, 255);
    SDL_RenderClear(map->graphRenderer);

    // Get window size
    int windowWidth, windowHeight;
    SDL_GetWindowSize(map->graphWindow, &windowWidth, &windowHeight);

    // Render the graph taking full window size with some margin
    int margin = 20;
    Graph_Render(&map->graphData, map->graphRenderer,
                 margin, margin,
                 windowWidth - 2 * margin,
                 windowHeight - 2 * margin);

    // Present the graph window
    SDL_RenderPresent(map->graphRenderer);
}
