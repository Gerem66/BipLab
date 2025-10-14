#ifndef UI_COMPONENTS_PROGRESSBAR_H
#define UI_COMPONENTS_PROGRESSBAR_H

#include <SDL2/SDL.h>

/**
 * Render a unified progress bar with rounded corners
 *
 * This is a generic UI component that can be used throughout the application
 * for any kind of progress visualization (health bars, loading progress, etc.)
 *
 * Example usage:
 *   SDL_Color bgColor = {35, 35, 40, 255};    // Dark background
 *   SDL_Color fgColor = {100, 255, 100, 255}; // Green foreground
 *   ProgressBar_Render(renderer, 100, 200, 300, 12,
 *                     0.75f, "Health: 75%", bgColor, fgColor);
 *
 * @param renderer SDL renderer
 * @param x X position
 * @param y Y position
 * @param width Bar width
 * @param height Bar height (recommended: 8-16px for modern look)
 * @param progress Progress value (0.0 to 1.0, automatically clamped)
 * @param label Text label above the bar (can be NULL for no label)
 * @param bgColor Background color
 * @param fgColor Foreground color
 */
void ProgressBar_Render(SDL_Renderer *renderer, int x, int y, int width, int height,
                       float progress, const char* label, SDL_Color bgColor, SDL_Color fgColor);

#endif // UI_COMPONENTS_PROGRESSBAR_H
