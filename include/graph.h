#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

// Graph configuration
#define GRAPH_HISTORY_MAX_SIZE 10000    // Maximum number of data points
#define GRAPH_TIMEOUT_FRAMES 100000     // Frames after which we force a graph update

// Forward declaration
typedef struct Map Map;

// Graph data structure
typedef struct {
    int *scoreHistory;              // Score history over time
    int *maxGenerationHistory;      // Max child generation history over time
    int historyCount;               // Number of data points
    int lastUpdateFrame;            // Frame of last graph update
} GraphData;

// Function declarations
bool Graph_Init(GraphData *graph);
void Graph_Free(GraphData *graph);
void Graph_AddPoint(GraphData *graph, Map *map);
void Graph_CheckTimeout(GraphData *graph, Map *map);
void Graph_Render(GraphData *graph, SDL_Renderer *renderer, int x, int y, int width, int height);

#endif // GRAPH_H
