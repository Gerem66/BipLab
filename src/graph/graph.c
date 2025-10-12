#include <stdlib.h>
#include <stdio.h>

#include "../../include/graph/graph.h"
#include "../../include/game.h"

bool Graph_Init(GraphData *graph) {
    if (graph == NULL) return false;

    graph->scoreHistory = malloc(GRAPH_HISTORY_MAX_SIZE * sizeof(int));
    if (graph->scoreHistory == NULL) {
        fprintf(stderr, "Failed to allocate memory for score history!\n");
        return false;
    }

    graph->maxGenerationHistory = malloc(GRAPH_HISTORY_MAX_SIZE * sizeof(int));
    if (graph->maxGenerationHistory == NULL) {
        fprintf(stderr, "Failed to allocate memory for max generation history!\n");
        free(graph->scoreHistory);
        return false;
    }

    graph->mutationHistory = malloc(GRAPH_HISTORY_MAX_SIZE * sizeof(float));
    if (graph->mutationHistory == NULL) {
        fprintf(stderr, "Failed to allocate memory for mutation history!\n");
        free(graph->scoreHistory);
        free(graph->maxGenerationHistory);
        return false;
    }

    graph->historyCount = 0;
    graph->circularIndex = 0;
    graph->lastUpdateFrame = 0;

    return true;
}

void Graph_Free(GraphData *graph) {
    if (graph == NULL) return;

    if (graph->scoreHistory != NULL) {
        free(graph->scoreHistory);
        graph->scoreHistory = NULL;
    }

    if (graph->maxGenerationHistory != NULL) {
        free(graph->maxGenerationHistory);
        graph->maxGenerationHistory = NULL;
    }

    if (graph->mutationHistory != NULL) {
        free(graph->mutationHistory);
        graph->mutationHistory = NULL;
    }

    graph->historyCount = 0;
    graph->lastUpdateFrame = 0;
}

void Graph_AddPoint(GraphData *graph, Map *map) {
    if (graph == NULL || map == NULL) return;

    // Find best score and max generation among living cells
    int bestScore = 0;
    int maxGeneration = 0;
    int validCellCount = 0;

    for (int i = 0; i < map->cellCount; ++i) {
        if (map->cells[i] != NULL) {
            validCellCount++;
            if (map->cells[i]->score > bestScore) {
                bestScore = map->cells[i]->score;
            }
            if (map->cells[i]->generation > maxGeneration) {
                maxGeneration = map->cells[i]->generation;
            }
        }
    }

    // Add point to graph (including mutation intensity)
    if (validCellCount > 0) {
        // Calculate mutation intensity as rate * probability (represents total change potential)
        float mutationIntensity = map->mutationParams.resetMutationRate * map->mutationParams.resetMutationProb;

        // Write to circular buffer
        graph->scoreHistory[graph->circularIndex] = bestScore;
        graph->maxGenerationHistory[graph->circularIndex] = maxGeneration;
        graph->mutationHistory[graph->circularIndex] = mutationIntensity;

        // Update circular index
        graph->circularIndex = (graph->circularIndex + 1) % GRAPH_HISTORY_MAX_SIZE;

        // Update count (max at GRAPH_HISTORY_MAX_SIZE)
        if (graph->historyCount < GRAPH_HISTORY_MAX_SIZE) {
            graph->historyCount++;
        }

        graph->lastUpdateFrame = map->frames;

        // Update max score if needed
        if (bestScore > map->maxScore) {
            map->maxScore = bestScore;
        }
    }
}

void Graph_CheckTimeout(GraphData *graph, Map *map) {
    if (graph == NULL || map == NULL) return;

    // Check timeout for graph update
    if (map->frames - graph->lastUpdateFrame >= GRAPH_TIMEOUT_FRAMES) {
        Graph_AddPoint(graph, map);
    }
}

void Graph_Render(GraphData *graph, SDL_Renderer *renderer, int x, int y, int width, int height) {
    if (graph == NULL || graph->scoreHistory == NULL || graph->maxGenerationHistory == NULL || graph->mutationHistory == NULL) {
        return;
    }

    SDL_Color bgColor = {0, 0, 0, 180};
    SDL_Color borderColor = {255, 255, 255, 255};
    SDL_Color scoreLineColor = {0, 255, 0, 255};      // Green for scores
    SDL_Color genLineColor = {255, 165, 0, 255};      // Orange for generations
    SDL_Color mutationLineColor = {255, 100, 255, 255}; // Magenta for mutation
    SDL_Color gridColor = {100, 100, 100, 100};

    // Draw background
    SDL_Rect bgRect = {x, y, width, height};
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(renderer, &bgRect);

    // Draw border
    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    SDL_RenderDrawRect(renderer, &bgRect);

    // Find maximum values for scaling
    int maxScore = 1;
    int maxGeneration = 1;
    float maxMutation = 0.01f; // Start with a reasonable minimum

    for (int i = 0; i < graph->historyCount; i++) {
        // Calculate chronological index (oldest to newest)
        int idx;
        if (graph->historyCount < GRAPH_HISTORY_MAX_SIZE) {
            // Buffer not full yet, simple indexing
            idx = i;
        } else {
            // Buffer is full, start from oldest data point
            idx = (graph->circularIndex + i) % GRAPH_HISTORY_MAX_SIZE;
        }

        if (graph->scoreHistory[idx] > maxScore) {
            maxScore = graph->scoreHistory[idx];
        }
        if (graph->maxGenerationHistory[idx] > maxGeneration) {
            maxGeneration = graph->maxGenerationHistory[idx];
        }
        if (graph->mutationHistory[idx] > maxMutation) {
            maxMutation = graph->mutationHistory[idx];
        }
    }

    // Draw horizontal grid lines
    SDL_SetRenderDrawColor(renderer, gridColor.r, gridColor.g, gridColor.b, gridColor.a);
    for (int i = 1; i < 5; i++) {
        int gridY = y + (height * i) / 5;
        SDL_RenderDrawLine(renderer, x, gridY, x + width, gridY);
    }

    // Draw score evolution curve (green)
    SDL_SetRenderDrawColor(renderer, scoreLineColor.r, scoreLineColor.g, scoreLineColor.b, scoreLineColor.a);
    for (int i = 1; i < graph->historyCount; i++) {
        // Get chronological indices
        int idx1, idx2;
        if (graph->historyCount < GRAPH_HISTORY_MAX_SIZE) {
            idx1 = i - 1;
            idx2 = i;
        } else {
            idx1 = (graph->circularIndex + i - 1) % GRAPH_HISTORY_MAX_SIZE;
            idx2 = (graph->circularIndex + i) % GRAPH_HISTORY_MAX_SIZE;
        }

        int x1 = x + ((i - 1) * width) / (graph->historyCount - 1);
        int y1 = y + height - (graph->scoreHistory[idx1] * height) / maxScore;
        int x2 = x + (i * width) / (graph->historyCount - 1);
        int y2 = y + height - (graph->scoreHistory[idx2] * height) / maxScore;

        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }

    // Draw max generation evolution curve (orange)
    SDL_SetRenderDrawColor(renderer, genLineColor.r, genLineColor.g, genLineColor.b, genLineColor.a);
    for (int i = 1; i < graph->historyCount; i++) {
        // Get chronological indices
        int idx1, idx2;
        if (graph->historyCount < GRAPH_HISTORY_MAX_SIZE) {
            idx1 = i - 1;
            idx2 = i;
        } else {
            idx1 = (graph->circularIndex + i - 1) % GRAPH_HISTORY_MAX_SIZE;
            idx2 = (graph->circularIndex + i) % GRAPH_HISTORY_MAX_SIZE;
        }

        int x1 = x + ((i - 1) * width) / (graph->historyCount - 1);
        int y1 = y + height - (graph->maxGenerationHistory[idx1] * height) / maxGeneration;
        int x2 = x + (i * width) / (graph->historyCount - 1);
        int y2 = y + height - (graph->maxGenerationHistory[idx2] * height) / maxGeneration;

        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }

    // Draw mutation evolution curve (magenta)
    SDL_SetRenderDrawColor(renderer, mutationLineColor.r, mutationLineColor.g, mutationLineColor.b, mutationLineColor.a);
    for (int i = 1; i < graph->historyCount; i++) {
        // Get chronological indices
        int idx1, idx2;
        if (graph->historyCount < GRAPH_HISTORY_MAX_SIZE) {
            idx1 = i - 1;
            idx2 = i;
        } else {
            idx1 = (graph->circularIndex + i - 1) % GRAPH_HISTORY_MAX_SIZE;
            idx2 = (graph->circularIndex + i) % GRAPH_HISTORY_MAX_SIZE;
        }

        int x1 = x + ((i - 1) * width) / (graph->historyCount - 1);
        int y1 = y + height - (int)((graph->mutationHistory[idx1] * height) / maxMutation);
        int x2 = x + (i * width) / (graph->historyCount - 1);
        int y2 = y + height - (int)((graph->mutationHistory[idx2] * height) / maxMutation);

        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }

    // Clean legend
    stringRGBA(renderer, x + 5, y + height + 5, "Green: Score | Orange: Max Generation | Magenta: Mutation", 200, 200, 200, 255);
}
