/**
 * @file realtime_graph.h
 * @brief Reusable real-time graph component for smooth data visualization
 *
 * This component provides smooth real-time graphing capabilities with
 * interpolation between data points to avoid jerky animations.
 */

#ifndef REALTIME_GRAPH_H
#define REALTIME_GRAPH_H

#include <SDL2/SDL.h>
#include <stdbool.h>

// Graph configuration
#define GRAPH_MAX_SAMPLES 256           // Maximum number of data samples
#define GRAPH_UPDATE_INTERVAL 0.25      // Update interval in seconds

// Graph color constants
#define GRAPH_BG_COLOR {35, 35, 40, 255}       // Background color
#define GRAPH_GRID_COLOR {60, 60, 65, 128}     // Grid lines color
#define GRAPH_TEXT_COLOR {255, 255, 255, 255}  // Text color

// Graph data structure
typedef struct {
    double samples[GRAPH_MAX_SAMPLES];  // Historical data samples
    int sampleCount;                    // Number of valid samples
    int currentIndex;                   // Current position in circular buffer
    double currentValue;                // Current raw value for display

    // Display properties
    char title[64];                     // Graph title
    SDL_Color color;                    // Graph color
    double minValue;                    // Minimum value for scaling (usually 0.0)
    double maxValue;                    // Maximum value for scaling (usually 100.0)
} RealtimeGraph;

/**
 * Initialize a real-time graph
 * @param graph Pointer to the graph structure
 * @param title Graph title
 * @param color Graph color
 * @param minValue Minimum value for Y-axis scaling
 * @param maxValue Maximum value for Y-axis scaling
 */
void RealtimeGraph_Init(RealtimeGraph* graph, const char* title, SDL_Color color,
                       double minValue, double maxValue);

/**
 * Add a new data sample to the graph
 * This should be called when new real data is available
 * @param graph Pointer to the graph structure
 * @param value New data value
 */
void RealtimeGraph_AddSample(RealtimeGraph* graph, double value);

/**
 * Render the real-time graph
 * @param renderer SDL renderer
 * @param graph Pointer to the graph structure
 * @param x X position
 * @param y Y position
 * @param width Graph width
 * @param height Graph height
 */
void RealtimeGraph_Render(SDL_Renderer* renderer, RealtimeGraph* graph,
                         int x, int y, int width, int height);

/**
 * Get the current raw value for display
 * @param graph Pointer to the graph structure
 * @return Current raw value
 */
double RealtimeGraph_GetCurrentValue(RealtimeGraph* graph);

/**
 * Clear all data from the graph
 * @param graph Pointer to the graph structure
 */
void RealtimeGraph_Clear(RealtimeGraph* graph);

#endif // REALTIME_GRAPH_H
