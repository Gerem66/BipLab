#ifndef UI_UTILS_H
#define UI_UTILS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

/**
 * Draw a smooth antialiased rounded rectangle
 * @param renderer SDL renderer
 * @param x X coordinate
 * @param y Y coordinate  
 * @param w Width
 * @param h Height
 * @param radius Corner radius
 * @param color Fill color
 */
void DrawSmoothRoundedRect(SDL_Renderer *renderer, int x, int y, int w, int h, int radius, SDL_Color color);

#endif // UI_UTILS_H
