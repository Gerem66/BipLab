#include "game.h"

void Game_render(SDL_Renderer *renderer, Map *map)
{
    Utils_setBackgroundColor(renderer, COLOR_DARK_GREEN);

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
    }

    // Render neural network
    if (map->renderNeuralNetwork)
    {
        int index = map->currentBestCellIndex;
        if (map->cells[index] != NULL)
            NeuralNetwork_Render(map->cells[index], renderer, index, 900, 400, 300, 400);
    }

    // Show messages
    if (map->renderText)
        Render_Text(map, COLOR_LIGHT_GRAY);

    // Zoom
    SDL_RenderSetViewport(renderer, &(SDL_Rect) {
        -map->viewOffset.x,
        -map->viewOffset.y,
        map->width,
        map->height
    });
    SDL_RenderSetScale(renderer, map->zoomFactor, map->zoomFactor);

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

    // Game informations
    time_t currentTime = time(NULL) - map->startTime;
    sprintf(message, "Time: %dm %ds (Frame %d)", (int)(currentTime / 60), (int)(currentTime % 60), map->frames % 1000);
    stringRGBA(map->renderer, 100, 25, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Generation: %d (max cell gen: %d)", map->generation, map->maxGeneration);
    stringRGBA(map->renderer, 100, 50, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Cells count: %d (total: %d)", aliveCount, map->cellCount - 1);
    stringRGBA(map->renderer, 100, 75, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Best score: %d (max: %d)", map->cells[map->currentBestCellIndex]->score, map->maxScore);
    stringRGBA(map->renderer, 100, 100, message, color.r, color.g, color.b, color.a);

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

    // Render controls
    sprintf(message, "N: Show/Hide neural network");
    stringRGBA(map->renderer, 850, 25, message, color.r, color.g, color.b, color.a);

    sprintf(message, "T: Show/Hide texts");
    stringRGBA(map->renderer, 850, 50, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Y: Enable/Disable render of rays");
    stringRGBA(map->renderer, 850, 75, message, color.r, color.g, color.b, color.a);

    sprintf(message, "I: Enable/Disable render of cells");
    stringRGBA(map->renderer, 850, 100, message, color.r, color.g, color.b, color.a);

    sprintf(message, "O: Enable/Disable vertical sync");
    stringRGBA(map->renderer, 850, 125, message, color.r, color.g, color.b, color.a);

    sprintf(message, "P: Pause/Unpause cells evolution");
    stringRGBA(map->renderer, 850, 150, message, color.r, color.g, color.b, color.a);

    sprintf(message, "R: Reset cells to last best cell");
    stringRGBA(map->renderer, 850, 175, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Return: Save best neural network");
    stringRGBA(map->renderer, 850, 200, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Mouse wheel: Zoom/Dezoom & move view");
    stringRGBA(map->renderer, 850, 225, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Esc: Quit");
    stringRGBA(map->renderer, 850, 250, message, color.r, color.g, color.b, color.a);
}
