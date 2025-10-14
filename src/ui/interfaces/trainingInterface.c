#include "../../../include/ui/interfaces/trainingInterface.h"
#include "../../../include/core/utils.h"
#include "../../../include/ui/ui_utils.h"
#include "../../../include/ui/graph/graphEvolution.h"
#include "../../../include/system/performance.h"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include <time.h>

#define TRAINING_GRAPH_WIDTH 400
#define TRAINING_GRAPH_HEIGHT 160

void TrainingInterface_RenderDashboard(SDL_Renderer *renderer, Map *map)
{
    // Set dark background for professional look
    Utils_setBackgroundColor(renderer, (SDL_Color){15, 15, 25, 255});

    // Always render, but only update text data every N frames to optimize performance
    bool shouldUpdateText = (map->frames % TRAINING_DASHBOARD_UPDATE_INTERVAL == 0);

    // Title bar
    SDL_Color titleColor = {100, 200, 255, 255};
    char titleText[100];
    sprintf(titleText, "BIPLAB TRAINING MODE - GENERATION %d", map->generation);
    stringRGBA(renderer, 20, 20, titleText, titleColor.r, titleColor.g, titleColor.b, titleColor.a);

    // Runtime information (top right)
    time_t effectiveTime = time(NULL) - map->startTime - map->pausedTime;
    int hours = effectiveTime / 3600;
    int minutes = (effectiveTime % 3600) / 60;
    int seconds = effectiveTime % 60;

    char timeText[100];
    if (hours > 0) {
        sprintf(timeText, "Runtime: %dh %dm %ds", hours, minutes, seconds);
    } else {
        sprintf(timeText, "Runtime: %dm %ds", minutes, seconds);
    }

    // Status indicator (positioned at top right with same spacing as title)
    SDL_Color statusColor = map->isRunning ? (SDL_Color){100, 255, 100, 255} : (SDL_Color){255, 100, 100, 255};
    char statusText[150];
    sprintf(statusText, "STATUS: %s - %s", map->isRunning ? "TRAINING" : "PAUSED", timeText);

    int statusX = TRAINING_SCREEN_WIDTH - 320;
    stringRGBA(renderer, statusX, 20, statusText, statusColor.r, statusColor.g, statusColor.b, statusColor.a);

    // Calculate column positions for 3-column layout
    int col1_x = 20;  // Text metrics column
    int col2_x = col1_x + 350;  // Graphs column

    // Render main metrics panel (column 1)
    TrainingInterface_RenderMetrics(renderer, map, col1_x, 80, shouldUpdateText);

    // Render graphs (column 2)
    TrainingInterface_RenderGraphs(renderer, map, col2_x, 80);

    // Performance warning if needed
    if (map->evolutionMetrics.generationsSinceImprovement > 100) {
        SDL_Color warningColor = {255, 200, 100, 255};
        if (map->evolutionMetrics.generationsSinceImprovement > 200) {
            warningColor = (SDL_Color){255, 100, 100, 255};
        }

        char warningText[100];
        sprintf(warningText, "⚠ STAGNATION DETECTED: %d generations without improvement",
                map->evolutionMetrics.generationsSinceImprovement);
        stringRGBA(renderer, 20, TRAINING_SCREEN_HEIGHT - 80, warningText,
                  warningColor.r, warningColor.g, warningColor.b, warningColor.a);
    }

    // Controls help
    SDL_Color helpColor = {150, 150, 150, 255};
    stringRGBA(renderer, 20, TRAINING_SCREEN_HEIGHT - 20, "M: Toggle Training Mode | P: Pause/Resume | R: Reset | C: Checkpoint | ESC: Quit",
              helpColor.r, helpColor.g, helpColor.b, helpColor.a);

    SDL_RenderPresent(renderer);
}

void TrainingInterface_RenderMetrics(SDL_Renderer *renderer, Map *map, int x, int y, bool updateData)
{
    SDL_Color labelColor = {200, 200, 200, 255};
    SDL_Color valueColor = {255, 255, 255, 255};
    SDL_Color goodColor = {100, 255, 100, 255};
    SDL_Color badColor = {255, 100, 100, 255};

    char text[150];
    int lineHeight = 25;
    int currentY = y - 20;

    // Static variables to cache expensive calculations
    static int cachedAliveCount = 0;
    static int lastUpdateFrame = -1;

    // Performance metrics
    sprintf(text, "PERFORMANCE METRICS");
    stringRGBA(renderer, x, currentY, text, labelColor.r, labelColor.g, labelColor.b, labelColor.a);
    currentY += lineHeight;

    // Count alive cells only when needed
    if (updateData || lastUpdateFrame != map->frames) {
        cachedAliveCount = 0;
        for (int i = 0; i < map->cellCount; ++i) {
            if (map->cells[i] != NULL && map->cells[i]->isAlive) {
                cachedAliveCount++;
            }
        }
        lastUpdateFrame = map->frames;
    }

    sprintf(text, "Best Score: %d | Max generation: %d", map->maxScore, map->maxGeneration);
    stringRGBA(renderer, x, currentY, text, valueColor.r, valueColor.g, valueColor.b, valueColor.a);
    currentY += lineHeight;

    sprintf(text, "FPS: %d | UPS: %d | GPS: %.2f", map->currentFPS, map->currentUPS, map->currentGPS);
    SDL_Color perfColor = (map->currentUPS > 1000) ? goodColor : valueColor;
    stringRGBA(renderer, x, currentY, text, perfColor.r, perfColor.g, perfColor.b, perfColor.a);
    currentY += lineHeight + 15;

    // Evolution metrics
    sprintf(text, "EVOLUTION METRICS");
    stringRGBA(renderer, x, currentY, text, labelColor.r, labelColor.g, labelColor.b, labelColor.a);
    currentY += lineHeight;

    sprintf(text, "Convergence Rate: %.4f", map->evolutionMetrics.convergenceRate);
    stringRGBA(renderer, x, currentY, text, valueColor.r, valueColor.g, valueColor.b, valueColor.a);
    currentY += lineHeight;

    sprintf(text, "Avg Score Improvement: %.4f", map->evolutionMetrics.avgScoreImprovement);
    SDL_Color impColor = (map->evolutionMetrics.avgScoreImprovement > 0.01f) ? goodColor :
                        (map->evolutionMetrics.avgScoreImprovement > 0.0f) ? valueColor : badColor;
    stringRGBA(renderer, x, currentY, text, impColor.r, impColor.g, impColor.b, impColor.a);
    currentY += lineHeight + 15;

    // Mutation parameters
    sprintf(text, "MUTATION PARAMETERS");
    stringRGBA(renderer, x, currentY, text, labelColor.r, labelColor.g, labelColor.b, labelColor.a);
    currentY += lineHeight;

    sprintf(text, "Mutation Rate: %.4f (Child: %.4f)",
            map->mutationParams.resetMutationRate, map->mutationParams.childMutationRate);
    stringRGBA(renderer, x, currentY, text, valueColor.r, valueColor.g, valueColor.b, valueColor.a);
    currentY += lineHeight;

    sprintf(text, "Mutation Prob: %.4f (Child: %.4f)",
            map->mutationParams.resetMutationProb, map->mutationParams.childMutationProb);
    stringRGBA(renderer, x, currentY, text, valueColor.r, valueColor.g, valueColor.b, valueColor.a);
    currentY += lineHeight + 15;

    // Network Information
    sprintf(text, "NETWORK INFO");
    stringRGBA(renderer, x, currentY, text, labelColor.r, labelColor.g, labelColor.b, labelColor.a);
    currentY += lineHeight;

    // Calculate network parameters
    int topology[] = NEURAL_NETWORK_TOPOLOGY;
    int layerCount = sizeof(topology) / sizeof(topology[0]);

    int totalWeights = 0;
    for (int i = 0; i < layerCount - 1; i++) {
        totalWeights += topology[i] * topology[i + 1];
    }
    int totalBiases = 0;
    for (int i = 1; i < layerCount; i++) {
        totalBiases += topology[i];
    }

    // Format large numbers with k/M suffix
    sprintf(text, "Architecture: %d layers", layerCount);
    stringRGBA(renderer, x, currentY, text, valueColor.r, valueColor.g, valueColor.b, valueColor.a);
    currentY += lineHeight;

    // Format large numbers with k/M suffix
    int totalParams = totalWeights + totalBiases;
    if (totalParams >= 1000000) {
        sprintf(text, "Parameters: %.1fM", totalParams / 1000000.0);
    } else if (totalParams >= 1000) {
        sprintf(text, "Parameters: %.1fk", totalParams / 1000.0);
    } else {
        sprintf(text, "Parameters: %d", totalParams);
    }
    stringRGBA(renderer, x, currentY, text, valueColor.r, valueColor.g, valueColor.b, valueColor.a);
}

void TrainingInterface_RenderGraphs(SDL_Renderer *renderer, Map *map, int x, int y)
{
    // Render main evolution graph
    Graph_Render(&map->graphData, renderer, x, y, TRAINING_GRAPH_WIDTH, TRAINING_GRAPH_HEIGHT);

    SDL_Color graphLabelColor = {200, 200, 200, 255};
    stringRGBA(renderer, x, y - 20, "Evolution Progress",
                graphLabelColor.r, graphLabelColor.g, graphLabelColor.b, graphLabelColor.a);

    // Simple progress bars for key metrics
    int barY = y + TRAINING_GRAPH_HEIGHT + 50;
    int barWidth = TRAINING_GRAPH_WIDTH;
    int barHeight = 8; // Very thin modern bars
    int barSpacing = 32;

    // Render performance breakdown bar (under metrics)
    TrainingInterface_RenderPerformanceBar(renderer, x, barY);
    barY += barSpacing;

    // Diversity progress bar
    SDL_Color barBgColor = {35, 35, 40, 255};
    SDL_Color barFgColor = {100, 180, 255, 255};
    int cornerRadius = 4; // Nice smooth corners for 8px height bars

    // Draw background
    DrawSmoothRoundedRect(renderer, x, barY, barWidth, barHeight, cornerRadius, barBgColor);

    // Draw foreground
    float diversityPercent = map->evolutionMetrics.diversityIndex; // Diversity is already 0-1
    int fgWidth = (int)(barWidth * diversityPercent);
    if (fgWidth > 0) {
        DrawSmoothRoundedRect(renderer, x, barY, fgWidth, barHeight, cornerRadius, barFgColor);
    }

    char barText[50];
    sprintf(barText, "Diversity: %.3f", map->evolutionMetrics.diversityIndex);
    stringRGBA(renderer, x, barY - 15, barText, 200, 200, 200, 255);

    // Best score progress (relative to max ever)
    barY += barSpacing;

    // Draw background
    DrawSmoothRoundedRect(renderer, x, barY, barWidth, barHeight, cornerRadius, barBgColor);

    float scorePercent = (map->maxScore > 0) ?
        (float)map->cells[map->currentBestCellIndex]->score / (float)map->maxScore : 0.0f;
    // Cap the percentage at 100% to prevent the bar from overflowing
    scorePercent = fminf(scorePercent, 1.0f);

    SDL_Color scoreFgColor = (scorePercent > 0.9f) ? (SDL_Color){100, 255, 100, 255} :
                            (scorePercent > 0.7f) ? (SDL_Color){255, 200, 100, 255} :
                            (SDL_Color){255, 100, 100, 255};

    int scoreFgWidth = (int)(barWidth * scorePercent);
    if (scoreFgWidth > 0) {
        DrawSmoothRoundedRect(renderer, x, barY, scoreFgWidth, barHeight, cornerRadius, scoreFgColor);
    }

    sprintf(barText, "Current best (%d) vs Best (%d): %.1f%%", map->cells[map->currentBestCellIndex]->score, map->maxScore, scorePercent * 100.0f);
    stringRGBA(renderer, x, barY - 15, barText, 200, 200, 200, 255);

    // Generation progress bar (alive cells / total initial cells)
    barY += barSpacing;

    // Draw background
    DrawSmoothRoundedRect(renderer, x, barY, barWidth, barHeight, cornerRadius, barBgColor);

    // Calculate alive count for this bar
    int aliveCount = 0;
    for (int i = 0; i < map->cellCount; ++i) {
        if (map->cells[i] != NULL && map->cells[i]->isAlive) {
            aliveCount++;
        }
    }

    // Invert the progress: full bar when all alive, empty when all dead
    float alivePercent = (float)aliveCount / (float)GAME_START_CELL_COUNT;
    float progressPercent = 1.0f - fminf(alivePercent, 1.0f); // Inverted and capped

    // Color logic inverted: green when generation is progressing (fewer cells alive)
    SDL_Color aliveFgColor = (progressPercent > 0.8f) ? (SDL_Color){100, 255, 100, 255} :
                            (progressPercent > 0.5f) ? (SDL_Color){255, 200, 100, 255} :
                            (progressPercent > 0.2f) ? (SDL_Color){255, 150, 100, 255} :
                            (SDL_Color){100, 150, 255, 255};

    int progressFgWidth = (int)(barWidth * progressPercent);
    if (progressFgWidth > 0) {
        DrawSmoothRoundedRect(renderer, x, barY, progressFgWidth, barHeight, cornerRadius, aliveFgColor);
    }

    sprintf(barText, "Generation Progress: %d/%d (%.1f%%)", aliveCount, GAME_START_CELL_COUNT, progressPercent * 100.0f);
    stringRGBA(renderer, x, barY - 15, barText, 200, 200, 200, 255);
}

void TrainingInterface_RenderPerformanceBar(SDL_Renderer *renderer, int x, int y)
{
    // Récupérer les statistiques de performance
    const PerfStats *nnStats = Perf_GetStats(PERF_NEURAL_NETWORK);
    const PerfStats *cellStats = Perf_GetStats(PERF_CELL_UPDATE);
    const PerfStats *mutationStats = Perf_GetStats(PERF_MUTATION);

    // Dimensions de la barre
    int barWidth = TRAINING_GRAPH_WIDTH;
    int barHeight = 8;
    int cornerRadius = 4;

    // Calculer les temps totaux
    double totalNN = (nnStats && nnStats->callCount > 0) ? nnStats->totalTime : 0.0;
    double totalCell = (cellStats && cellStats->callCount > 0) ? cellStats->totalTime : 0.0;
    double totalMutation = (mutationStats && mutationStats->callCount > 0) ? mutationStats->totalTime : 0.0;
    double totalTime = totalNN + totalCell + totalMutation;

    if (totalTime <= 0) {
        // Pas de données, afficher une barre grise
        SDL_Color grayColor = {100, 100, 100, 255};
        DrawSmoothRoundedRect(renderer, x, y, barWidth, barHeight, cornerRadius, grayColor);

        SDL_Color labelColor = {200, 200, 200, 255};
        stringRGBA(renderer, x, y - barHeight - 8, "No performance data yet...",
                   labelColor.r, labelColor.g, labelColor.b, labelColor.a);
        return;
    }

    // Calculer les proportions
    double nnProportion = totalNN / totalTime;
    double cellProportion = totalCell / totalTime;
    double mutationProportion = totalMutation / totalTime;

    // Couleurs pour chaque type
    SDL_Color nnColor = {100, 150, 255, 255};      // Bleu pour Neural Network
    SDL_Color cellColor = {100, 255, 150, 255};     // Vert pour Cell Update
    SDL_Color mutationColor = {255, 150, 100, 255}; // Orange pour Mutation

    // Fond noir arrondi
    SDL_Color bgColor = {35, 35, 40, 255};
    DrawSmoothRoundedRect(renderer, x, y, barWidth, barHeight, cornerRadius, bgColor);

    // Dessiner les segments proportionnels
    int currentX = x;

    // Segment Neural Network
    int nnWidth = (int)(barWidth * nnProportion);
    if (nnWidth > 0) {
        // Dessiner avec coins arrondis seulement à gauche si c'est le premier segment
        DrawSmoothRoundedRect(renderer, currentX, y, nnWidth, barHeight, cornerRadius, nnColor);
        currentX += nnWidth;
    }

    // Segment Cell Update
    int cellWidth = (int)(barWidth * cellProportion);
    if (cellWidth > 0) {
        // Rectangle simple au milieu
        SDL_Rect cellRect = {currentX, y, cellWidth, barHeight};
        SDL_SetRenderDrawColor(renderer, cellColor.r, cellColor.g, cellColor.b, cellColor.a);
        DrawSmoothRoundedRect(renderer, cellRect.x, cellRect.y, cellRect.w, cellRect.h, cornerRadius, cellColor);
        currentX += cellWidth;
    }

    // Segment Mutation (le reste de la barre)
    int mutationWidth = (x + barWidth) - currentX;
    if (mutationWidth > 0) {
        // Dessiner avec coins arrondis seulement à droite si c'est le dernier segment
        DrawSmoothRoundedRect(renderer, currentX, y, mutationWidth, barHeight, cornerRadius, mutationColor);
    }

    // Légendes sous la barre
    SDL_Color textColor = {255, 255, 255, 200};
    int legendY = y - barHeight - 8;

    char legendText[200];
    sprintf(legendText, "NN: %.1f%% | Cells: %.1f%% | Mutation: %.1f%%",
            nnProportion * 100.0,
            cellProportion * 100.0,
            mutationProportion * 100.0);
    stringRGBA(renderer, x, legendY, legendText, textColor.r, textColor.g, textColor.b, textColor.a);
}
