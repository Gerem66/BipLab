#ifndef GRAPH_WINDOW_H
#define GRAPH_WINDOW_H

#include <SDL2/SDL.h>
#include "../../core/game.h"

// Forward declaration to avoid circular dependency
typedef struct Map Map;

/**
 * Create and show a separate window for displaying the evolution graph
 * @param map Pointer to the game map structure
 * @return true if window was created successfully, false otherwise
 */
bool GraphWindow_Create(Map *map);

/**
 * Destroy the graph window and clean up resources
 * @param map Pointer to the game map structure
 */
void GraphWindow_Destroy(Map *map);

/**
 * Render the graph in the separate window
 * @param map Pointer to the game map structure
 */
void GraphWindow_Render(Map *map);

#endif // GRAPH_WINDOW_H
