#include "game.h"

void Game_render(SDL_Renderer *renderer, Map *map)
{
    Utils_setBackgroundColor(renderer, COLOR_DARK_GREEN);

    // === WORLD RENDERING (with zoom/pan) ===

    // Apply transformations for world objects
    SDL_RenderSetViewport(renderer, &(SDL_Rect) {
        -map->viewOffset.x,
        -map->viewOffset.y,
        map->width,
        map->height
    });
    SDL_RenderSetScale(renderer, map->zoomFactor, map->zoomFactor);

    if (map->renderEnabled)
    {
        // Render foods
        for (int i = 0; i < GAME_START_FOOD_COUNT; ++i)
            Food_render(map->foods[i], map->renderer, map->renderText);

        // Render cells
        for (int i = 0; i < map->cellCount; ++i)
            if (map->cells[i] != NULL)
                Cell_render(map->cells[i], map->renderer, map->renderRays, i == map->currentBestCellIndex);

        // Render walls
        for (int i = 0; i < GAME_START_WALL_COUNT; ++i)
            Wall_render(map->walls[i], map->renderer);
    } else {
        // Render only the best cell when others are hidden
        if (map->cells[map->currentBestCellIndex] != NULL)
            Cell_render(map->cells[map->currentBestCellIndex], map->renderer, map->renderRays, true);
    }

    // === UI RENDERING (screen-fixed) ===

    // Reset transformations for UI elements
    SDL_RenderSetViewport(renderer, NULL);
    SDL_RenderSetScale(renderer, 1.0f, 1.0f);

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
        Render_ScoreGraph(map, renderer, 50, 550, 400, 200);
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

    // Frames de la génération actuelle (frames de la génération précédente)
    sprintf(message, "Frames: %d (%d prev gen)", map->frames, map->previousGenFrames);
    stringRGBA(map->renderer, 100, 50, message, color.r, color.g, color.b, color.a);

    // FPS (rendu), UPS (update) et GPS (générations)
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
    stringRGBA(map->renderer, 850, 120, message, color.r, color.g, color.b, color.a);

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

void Render_ScoreGraph(Map *map, SDL_Renderer *renderer, int x, int y, int width, int height)
{
    if (map->scoreHistory == NULL) {
        return;
    }

    SDL_Color bgColor = {0, 0, 0, 180};
    SDL_Color borderColor = {255, 255, 255, 255};
    SDL_Color lineColor = {0, 255, 0, 255};
    SDL_Color gridColor = {100, 100, 100, 100};

    // Draw background
    SDL_Rect bgRect = {x, y, width, height};
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(renderer, &bgRect);

    // Draw border
    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    SDL_RenderDrawRect(renderer, &bgRect);

    // Find maximum score for scaling
    int maxScore = 1;
    for (int i = 0; i < map->scoreHistoryCount; i++) {
        if (map->scoreHistory[i] > maxScore) {
            maxScore = map->scoreHistory[i];
        }
    }

    // Draw horizontal grid lines
    SDL_SetRenderDrawColor(renderer, gridColor.r, gridColor.g, gridColor.b, gridColor.a);
    for (int i = 1; i < 5; i++) {
        int gridY = y + (height * i) / 5;
        SDL_RenderDrawLine(renderer, x, gridY, x + width, gridY);
    }

    // Draw evolution curve
    SDL_SetRenderDrawColor(renderer, lineColor.r, lineColor.g, lineColor.b, lineColor.a);

    for (int i = 1; i < map->scoreHistoryCount; i++) {
        int x1 = x + ((i - 1) * width) / (map->scoreHistoryCount - 1);
        int y1 = y + height - (map->scoreHistory[i - 1] * height) / maxScore;
        int x2 = x + (i * width) / (map->scoreHistoryCount - 1);
        int y2 = y + height - (map->scoreHistory[i] * height) / maxScore;

        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }

    // Title and labels
    char message[100];
    sprintf(message, "Complete Evolution - Gen %d (Max: %d)", map->scoreHistoryCount, maxScore);
    stringRGBA(renderer, x + 5, y - 20, message, 255, 255, 255, 255);
}
