#include "../../../include/ui/interfaces/gameInterface.h"
#include "../../../include/ui/ui.h"
#include "../../../include/core/utils.h"
#include "../../../include/ui/graph/neuralNetworkRender.h"
#include "../../../include/ui/graph/graphEvolution.h"
#include "../../../include/entities/food.h"
#include "../../../include/entities/wall.h"
#include "../../../include/entities/cell.h"
#include "../../../include/ui/interfaces/trainingInterface.h"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include <time.h>

void GameInterface_Render(SDL_Renderer *renderer, Map *map)
{
    // Use training dashboard if training mode is enabled
    if (map->mode == SCREEN_TRAINING) {
        TrainingInterface_RenderDashboard(renderer, map);
        return;
    }

    Utils_setBackgroundColor(renderer, COLOR_DARK_GREEN);

    // === WORLD RENDERING (with zoom/pan) ===

    // Save current transformations
    float scaleX, scaleY;
    SDL_RenderGetScale(renderer, &scaleX, &scaleY);

    // Apply zoom and panning for world objects
    SDL_RenderSetScale(renderer, map->zoomFactor, map->zoomFactor);

    // Calculate coordinates with offset for rendering
    int offsetX = (int)(-map->viewOffset.x);
    int offsetY = (int)(-map->viewOffset.y);

    if (map->renderEnabled)
    {
        // Render foods with offset
        for (int i = 0; i < GAME_START_FOOD_COUNT; ++i) {
            // Save original position
            SDL_FRect originalRect = map->foods[i]->rect;
            // Apply offset
            map->foods[i]->rect.x += offsetX;
            map->foods[i]->rect.y += offsetY;
            Food_render(map->foods[i], map->renderer, map->renderText);
            // Restore original position
            map->foods[i]->rect = originalRect;
        }

        // Render cells with offset
        for (int i = 0; i < map->cellCount; ++i) {
            if (map->cells[i] != NULL) {
                // Save original position
                SDL_FPoint originalPos = map->cells[i]->position;
                // Apply offset
                map->cells[i]->position.x += offsetX;
                map->cells[i]->position.y += offsetY;
                Cell_render(map->cells[i], map->renderer, map->renderRays, i == map->currentBestCellIndex);
                // Restore original position
                map->cells[i]->position = originalPos;
            }
        }

        // Render walls with offset
        for (int i = 0; i < GAME_START_WALL_COUNT; ++i) {
            // Save original position
            SDL_FRect originalRect = map->walls[i]->rect;
            // Apply offset
            map->walls[i]->rect.x += offsetX;
            map->walls[i]->rect.y += offsetY;
            Wall_render(map->walls[i], map->renderer);
            // Restore original position
            map->walls[i]->rect = originalRect;
        }
    } else {
        // Render only the best cell when others are hidden
        if (map->cells[map->currentBestCellIndex] != NULL) {
            // Save original position
            SDL_FPoint originalPos = map->cells[map->currentBestCellIndex]->position;
            // Apply offset
            map->cells[map->currentBestCellIndex]->position.x += offsetX;
            map->cells[map->currentBestCellIndex]->position.y += offsetY;
            Cell_render(map->cells[map->currentBestCellIndex], map->renderer, map->renderRays, true);
            // Restore original position
            map->cells[map->currentBestCellIndex]->position = originalPos;
        }
    }

    // === UI RENDERING (screen-fixed) ===

    // Restore original transformations for UI
    SDL_RenderSetScale(renderer, scaleX, scaleY);

    // Neural network visualization
    if (map->renderNeuralNetwork)
    {
        int index = map->currentBestCellIndex;
        if (map->cells[index] != NULL)
            NeuralNetworkRender_Draw(map->cells[index], renderer, index, 900, 400, 300, 400);
    }

    // Score evolution graph
    if (map->renderScoreGraph)
    {
        Graph_Render(&map->graphData, renderer, 50, 550, 400, 200);
    }

    // Text information overlay
    if (map->renderText)
        GameInterface_RenderText(map, COLOR_LIGHT_GRAY);

    // Update screen
    SDL_RenderPresent(renderer);
}

void GameInterface_RenderText(Map *map, SDL_Color color)
{
    char message[100];

    int aliveCount = 0;
    for (int i = 0; i < map->cellCount; ++i)
        if (map->cells[i] != NULL && map->cells[i]->isAlive)
            aliveCount++;

    //
    // Left column - Game info
    //

    // Time
    time_t effectiveTime = time(NULL) - map->startTime - map->pausedTime;
    sprintf(message, "Time: %dm %ds", (int)(effectiveTime / 60), (int)(effectiveTime % 60));
    stringRGBA(map->renderer, 100, 25, message, color.r, color.g, color.b, color.a);

    // Current generation frames (previous generation frames)
    sprintf(message, "Frame: %d (%d prev gen)", map->frames, map->previousGenFrames);
    stringRGBA(map->renderer, 100, 50, message, color.r, color.g, color.b, color.a);

    // FPS (render), UPS (update) and GPS (generations)
    sprintf(message, "FPS: %d | UPS: %d | GPS: %.2f", map->currentFPS, map->currentUPS, map->currentGPS);
    stringRGBA(map->renderer, 100, 75, message, color.r, color.g, color.b, color.a);

    // Checkpoint informations
    int gensSinceCheckpoint = map->generation - map->lastCheckpointGeneration;
    sprintf(message, "Checkpoints: %d saved (next in %d gen)", map->checkpointCounter, CHECKPOINT_SAVE_INTERVAL - gensSinceCheckpoint);
    stringRGBA(map->renderer, 100, 100, message, color.r, color.g, color.b, color.a);

    // Zoom level indicator (only when zoomed)
    sprintf(message, "Zoom: %.2fx", map->zoomFactor);
    stringRGBA(map->renderer, 100, 125, message, color.r, color.g, color.b, color.a);
    GameInterface_RenderZoomBar(map, color, 100, 145);

    //
    // Middle column - Cells info
    //

    // Generation and cells info
    sprintf(message, "Generation: %d (max cell gen: %d)", map->generation, map->maxGeneration);
    stringRGBA(map->renderer, 500, 25, message, color.r, color.g, color.b, color.a);

    // Cells count
    sprintf(message, "Cells count: %d (total: %d)", aliveCount, map->cellCount);
    stringRGBA(map->renderer, 500, 50, message, color.r, color.g, color.b, color.a);

    // Best score
    sprintf(message, "Best score: %d (max: %d)", map->cells[map->currentBestCellIndex]->score, map->maxScore);
    stringRGBA(map->renderer, 500, 75, message, color.r, color.g, color.b, color.a);

    // Diversity and convergence
    sprintf(message, "Diversity: %.3f | Convergence: %.3f", map->evolutionMetrics.diversityIndex, map->evolutionMetrics.convergenceRate);
    stringRGBA(map->renderer, 500, 100, message, color.r, color.g, color.b, color.a);

    // Evolution info (improved display)
    sprintf(message, "Mutation Rate: %.3f (child: %.3f)", map->mutationParams.resetMutationRate, map->mutationParams.childMutationRate);
    stringRGBA(map->renderer, 500, 125, message, color.r, color.g, color.b, color.a);

    // Evolution probability info
    sprintf(message, "Mutation Prob: %.3f (child: %.3f)", map->mutationParams.resetMutationProb, map->mutationParams.childMutationProb);
    stringRGBA(map->renderer, 500, 150, message, color.r, color.g, color.b, color.a);

    // Color-coded stagnation warning
    SDL_Color stagnationColor = color;
    if (map->evolutionMetrics.generationsSinceImprovement > 200) {
        stagnationColor = (SDL_Color){255, 100, 100, 255}; // Red for emergency
    } else if (map->evolutionMetrics.generationsSinceImprovement > 100) {
        stagnationColor = (SDL_Color){255, 200, 100, 255}; // Orange for warning
    }
    sprintf(message, "Stagnation: %d gen | AvgImpr: %.4f", map->evolutionMetrics.generationsSinceImprovement, map->evolutionMetrics.avgScoreImprovement);
    stringRGBA(map->renderer, 500, 175, message, stagnationColor.r, stagnationColor.g, stagnationColor.b, stagnationColor.a);

    //
    // Optional player info
    //

#if CELL_AS_PLAYER
    // Player informations
    sprintf(message, "Player pos: %d, %d", (int)map->cells[0]->position.x, (int)map->cells[0]->position.y);
    stringRGBA(map->renderer, 500, 225, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Angle: %f", map->cells[0]->angle);
    stringRGBA(map->renderer, 500, 250, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Speed: %f", map->cells[0]->speed);
    stringRGBA(map->renderer, 500, 275, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Score: %d", map->cells[0]->score);
    stringRGBA(map->renderer, 500, 300, message, color.r, color.g, color.b, color.a);
#endif

    //
    // Right column - Render controls
    //

    sprintf(message, "N: Show/Hide neural network");
    stringRGBA(map->renderer, 850, 25, message, color.r, color.g, color.b, color.a);

    sprintf(message, "G: Show/Hide score graph (+ shift: new window)");
    stringRGBA(map->renderer, 850, 50, message, color.r, color.g, color.b, color.a);

    sprintf(message, "T: Show/Hide texts");
    stringRGBA(map->renderer, 850, 75, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Y: Enable/Disable render of rays");
    stringRGBA(map->renderer, 850, 100, message, color.r, color.g, color.b, color.a);

    sprintf(message, "I: Enable/Disable render of cells");
    stringRGBA(map->renderer, 850, 125, message, color.r, color.g, color.b, color.a);

    sprintf(message, "V: Enable/Disable vertical sync");
    stringRGBA(map->renderer, 850, 150, message, color.r, color.g, color.b, color.a);

    sprintf(message, "P: Pause/Unpause cells evolution");
    stringRGBA(map->renderer, 850, 175, message, color.r, color.g, color.b, color.a);

    sprintf(message, "R: Reset cells to last best cell");
    stringRGBA(map->renderer, 850, 200, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Return: Save best neural network");
    stringRGBA(map->renderer, 850, 225, message, color.r, color.g, color.b, color.a);

    sprintf(message, "C: Save checkpoint manually");
    stringRGBA(map->renderer, 850, 250, message, color.r, color.g, color.b, color.a);

    sprintf(message, "M: Toggle training mode");
    stringRGBA(map->renderer, 850, 275, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Mouse wheel: Zoom/Dezoom & move view");
    stringRGBA(map->renderer, 850, 300, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Esc: Quit");
    stringRGBA(map->renderer, 850, 325, message, color.r, color.g, color.b, color.a);
}

void GameInterface_RenderZoomBar(Map *map, SDL_Color color, int x, int y)
{
    // Only show zoom bar when zoomed (not at default 1.0x)
    if (fabs(map->zoomFactor - 1.0f) < 0.01f) {
        return; // Don't show bar at default zoom
    }

    // Zoom bar (visual representation)
    int barWidth = 150;
    int barHeight = 8;

    // Background bar (dark)
    SDL_Rect bgRect = {x, y, barWidth, barHeight};
    SDL_SetRenderDrawColor(map->renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(map->renderer, &bgRect);

    // Border
    SDL_SetRenderDrawColor(map->renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawRect(map->renderer, &bgRect);

    // Fill bar based on zoom level (range 0.1x to 5.0x)
    float minZoom = 0.1f;
    float maxZoom = 5.0f;
    float clampedZoom = fmaxf(minZoom, fminf(maxZoom, map->zoomFactor));
    float normalizedZoom = (clampedZoom - minZoom) / (maxZoom - minZoom);

    int fillWidth = (int)(normalizedZoom * (barWidth - 2));
    if (fillWidth > 0) {
        SDL_Rect fillRect = {x + 1, y + 1, fillWidth, barHeight - 2};

        // Color based on zoom level
        if (map->zoomFactor < 0.5f) {
            SDL_SetRenderDrawColor(map->renderer, 100, 150, 255, 255); // Blue for zoom out
        } else if (map->zoomFactor > 2.0f) {
            SDL_SetRenderDrawColor(map->renderer, 255, 150, 100, 255); // Orange for zoom in
        } else {
            SDL_SetRenderDrawColor(map->renderer, 100, 255, 100, 255); // Green for normal
        }

        SDL_RenderFillRect(map->renderer, &fillRect);
    }
}
