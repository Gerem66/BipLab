#ifndef UI_UTILS_H
#define UI_UTILS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#define PI 3.14159265358979323846

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

/**
 * Draw a filled circle using midpoint circle algorithm
 * @param rend SDL renderer
 * @param x0 Center X coordinate
 * @param y0 Center Y coordinate
 * @param radius Circle radius
 */
void SDL_RenderFillCircle(SDL_Renderer* rend, int x0, int y0, int radius);

/**
 * Draw an arc
 * @param rend SDL renderer
 * @param x0 Center X coordinate
 * @param y0 Center Y coordinate
 * @param radius Arc radius
 * @param startAngle Start angle in degrees
 * @param endAngle End angle in degrees
 */
void SDL_RenderDrawArc(SDL_Renderer* rend, int x0, int y0, int radius, int startAngle, int endAngle);

/**
 * Draw a filled circle (alternative implementation)
 * @param renderer SDL renderer
 * @param x Center X coordinate
 * @param y Center Y coordinate
 * @param radius Circle radius
 */
void SDL_RenderDrawCircle(SDL_Renderer *renderer, int x, int y, int radius);

/**
 * Draw a circle outline
 * @param renderer SDL renderer
 * @param x Center X coordinate
 * @param y Center Y coordinate
 * @param radius Circle radius
 */
void SDL_RenderDrawCircleOutline(SDL_Renderer *renderer, int x, int y, int radius);

#endif // UI_UTILS_H
