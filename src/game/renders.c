#include "game.h"

void Game_render(SDL_Renderer *renderer, Map *map)
{
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
            NeuralNetwork_Render(map->cells[index], renderer, index, 900, 400, 300, 400);
    }

    // Score evolution graph
    if (map->renderScoreGraph)
    {
        Graph_Render(&map->graphData, renderer, 50, 550, 400, 200);
    }

    // Text information overlay
    if (map->renderText)
        Render_Text(map, COLOR_LIGHT_GRAY);

    // Update screen
    SDL_RenderPresent(renderer);
}

void Render_Text(Map *map, SDL_Color color)
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

    //
    // Middle column - Cells info
    //

    // Generation and cells info
    sprintf(message, "Generation: %d (max cell gen: %d)", map->generation, map->maxGeneration);
    stringRGBA(map->renderer, 500, 25, message, color.r, color.g, color.b, color.a);

    // Cells count
    sprintf(message, "Cells count: %d (total: %d)", aliveCount, map->cellCount - 1);
    stringRGBA(map->renderer, 500, 50, message, color.r, color.g, color.b, color.a);

    // Best score
    sprintf(message, "Best score: %d (max: %d)", map->cells[map->currentBestCellIndex]->score, map->maxScore);
    stringRGBA(map->renderer, 500, 75, message, color.r, color.g, color.b, color.a);

    //
    // Optional player info
    //

#if CELL_AS_PLAYER
    // Player informations
    sprintf(message, "Player pos: %d, %d", (int)map->cells[0]->position.x, (int)map->cells[0]->position.y);
    stringRGBA(map->renderer, 500, 25, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Angle: %f", map->cells[0]->angle);
    stringRGBA(map->renderer, 500, 75, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Speed: %f", map->cells[0]->speed);
    stringRGBA(map->renderer, 500, 50, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Score: %d", map->cells[0]->score);
    stringRGBA(map->renderer, 500, 100, message, color.r, color.g, color.b, color.a);
#endif

    //
    // Right column - Render controls
    //

    sprintf(message, "N: Show/Hide neural network");
    stringRGBA(map->renderer, 850, 25, message, color.r, color.g, color.b, color.a);

    sprintf(message, "G: Show/Hide score graph");
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

    sprintf(message, "Mouse wheel: Zoom/Dezoom & move view");
    stringRGBA(map->renderer, 850, 275, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Esc: Quit");
    stringRGBA(map->renderer, 850, 300, message, color.r, color.g, color.b, color.a);
}
