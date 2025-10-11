#ifndef NEURAL_NETWORK_RENDER_H
#define NEURAL_NETWORK_RENDER_H

#include <SDL2/SDL.h>
#include "../cell.h"

/**
 * Render the neural network visualization for a cell
 * @param cell Pointer to the cell whose neural network to render
 * @param renderer SDL renderer to use for drawing
 * @param index Index of the cell (for display purposes)
 * @param x X position for the visualization
 * @param y Y position for the visualization
 * @param w Width of the visualization area
 * @param h Height of the visualization area
 */
void NeuralNetworkRender_Draw(Cell *cell, SDL_Renderer *renderer, int index, int x, int y, int w, int h);

#endif // NEURAL_NETWORK_RENDER_H
