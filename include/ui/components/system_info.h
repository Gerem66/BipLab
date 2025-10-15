/**
 * @file system_info.h
 * @brief System information UI component for the training interface
 *
 * This component provides comprehensive system information display including
 * hardware specifications and real-time usage graphs for CPU, RAM, and GPU.
 */

#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include <SDL2/SDL.h>
#include "../../core/game.h"

// Configuration constants
#define SYSTEM_INFO_PANEL_WIDTH 350        // Width of the system info panel
#define SYSTEM_INFO_GRAPH_HEIGHT 60        // Height of individual usage graphs
#define SYSTEM_INFO_LINE_HEIGHT 20         // Height of text lines
#define SYSTEM_INFO_MARGIN 10              // Margin between elements

// Graph colors
#define CPU_GRAPH_COLOR     {100, 150, 255, 255}   // Blue for CPU
#define RAM_GRAPH_COLOR     {100, 255, 150, 255}   // Green for RAM
#define GPU_GRAPH_COLOR     {255, 150, 100, 255}   // Orange for GPU
#define GRAPH_BG_COLOR      {35, 35, 40, 255}      // Dark background
#define GRAPH_GRID_COLOR    {60, 60, 65, 128}      // Grid lines

/**
 * Render the complete system information panel
 * This includes hardware specifications and real-time usage graphs
 *
 * @param renderer SDL renderer
 * @param map Game map (for frame timing)
 * @param x X position of the panel
 * @param y Y position of the panel
 */
void SystemInfo_RenderPanel(SDL_Renderer *renderer, Map *map, int x, int y);

/**
 * Render hardware specifications section
 * Shows CPU model, RAM size, GPU model, etc.
 *
 * @param renderer SDL renderer
 * @param x X position
 * @param y Y position
 * @return Next Y position after rendering
 */
int SystemInfo_RenderSpecs(SDL_Renderer *renderer, int x, int y);

/**
 * Render real-time usage graphs
 * Shows CPU, RAM, and GPU usage over time
 *
 * @param renderer SDL renderer
 * @param x X position
 * @param y Y position
 * @return Next Y position after rendering
 */
int SystemInfo_RenderUsageGraphs(SDL_Renderer *renderer, int x, int y);

/**
 * Helper function to get color based on usage percentage
 * Returns green for low usage, yellow for medium, red for high
 *
 * @param usage Usage percentage (0.0-100.0)
 * @return SDL_Color representing the usage level
 */
SDL_Color SystemInfo_GetUsageColor(double usage);

/**
 * Helper function to format percentage with appropriate precision
 *
 * @param value Percentage value (0.0-100.0)
 * @param buffer Output buffer (must be at least 16 bytes)
 * @return Pointer to the buffer
 */
char* SystemInfo_FormatPercentage(double value, char* buffer);

#endif // SYSTEM_INFO_H
