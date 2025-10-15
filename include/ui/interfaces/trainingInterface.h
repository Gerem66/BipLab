#ifndef TRAINING_INTERFACE_H
#define TRAINING_INTERFACE_H

#include <SDL2/SDL.h>
#include "../../core/game.h"
#include "../components/button.h"

#define TRAINING_GRAPH_WIDTH 350
#define TRAINING_GRAPH_HEIGHT 160

/**
 * Render the training dashboard
 * @param renderer SDL renderer
 * @param map Game map
 */
void TrainingInterface_RenderDashboard(SDL_Renderer *renderer, Map *map);

/**
 * Render training metrics panel
 * @param renderer SDL renderer
 * @param map Game map
 * @param x X position
 * @param y Y position
 * @param updateData Whether to update cached data
 */
void TrainingInterface_RenderMetrics(SDL_Renderer *renderer, Map *map, int x, int y, bool updateData);

/**
 * Render training graphs panel
 * @param renderer SDL renderer
 * @param map Game map
 * @param x X position
 * @param y Y position
 */
void TrainingInterface_RenderGraphs(SDL_Renderer *renderer, Map *map, int x, int y);

/**
 * Render performance breakdown bar
 * @param renderer SDL renderer
 * @param map Game map
 * @param x X position
 * @param y Y position
 */
void TrainingInterface_RenderPerformanceBar(SDL_Renderer *renderer, int x, int y);

/**
 * Handle mouse events in training interface
 * Handles clicks on UI elements like the multithreading toggle button
 * @param map Game map containing UI state
 * @param mouseX Mouse X position
 * @param mouseY Mouse Y position
 * @param mousePressed Mouse button state (true if left button is pressed)
 */
void TrainingInterface_HandleMouseEvents(Map *map, int mouseX, int mouseY, bool mousePressed);

#endif // TRAINING_INTERFACE_H
