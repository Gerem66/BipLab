#include "game.h"

void Game_reset(Map *map, bool fullReset)
{
    // Get best cell
    Cell *bestCell;

    if (fullReset)
    {
        bestCell = map->bestCellEver;
    }
    else
    {
        bestCell = map->cells[0];
        for (int i = 0; i < map->cellCount; ++i)
            if (map->cells[i] != NULL && map->cells[i]->score > bestCell->score)
                bestCell = map->cells[i];

        if (bestCell->score > map->maxScore)
            map->maxScore = bestCell->score;
    }

    // Reset & mutate firsts cells state
    int revived = 0;
    while (revived < GAME_START_CELL_COUNT)
    {
        int bestIndex = -1;
        int bestScore = -1;
        for (int i = 0; i < map->cellCount; ++i)
        {
            if (map->cells[i] == NULL)
                continue;

            if (!map->cells[i]->isAlive && map->cells[i]->score > bestScore)
            {
                bestIndex = i;
                bestScore = map->cells[i]->score;
            }
        }
        if (bestIndex == -1)
        {
            fprintf(stderr, "Error while reviving cells !\n");
            return;
        }

        NeuralNetwork *newNN = NeuralNetwork_Copy(bestCell->nn);
        if (newNN == NULL)
        {
            fprintf(stderr, "Failed to copy NeuralNetwork !\n");
            return;
        }

        Cell_reset(map->cells[bestIndex]);
        freeNeuralNetwork(map->cells[bestIndex]->nn);
        map->cells[bestIndex]->nn = newNN;
        Cell_mutate(
            map->cells[bestIndex],
            NEURAL_NETWORK_RESET_MUTATION_RATE,
            NEURAL_NETWORK_RESET_MUTATION_PROB
        );

        revived++;
    }

    // Reset foods state
    for (int i = 0; i < GAME_START_FOOD_COUNT; ++i)
        Food_reset(map->foods[i], map);

    // Reset walls state
    for (int i = 0; i < GAME_START_WALL_COUNT; ++i)
        Wall_reset(map->walls[i], map);

    // Sauvegarder les frames de la génération qui se termine
    map->previousGenFrames = map->frames;

    // Calculer le GPS (Generations Per Second)
    static time_t lastGenTime = 0;
    static int generationCount = 0;
    static time_t firstGenTime = 0;

    time_t effectiveTime = time(NULL) - map->startTime - map->pausedTime;
    if (firstGenTime == 0) {
        firstGenTime = effectiveTime;
    }

    generationCount++;
    if (effectiveTime > firstGenTime && effectiveTime != lastGenTime) {
        map->currentGPS = (float)generationCount / (float)(effectiveTime - firstGenTime);
    }
    lastGenTime = effectiveTime;

    map->frames = 1;
    map->generation++;

    // Check if you have to make a checkpoint at each generation
    if (map->generation - map->lastCheckpointGeneration >= CHECKPOINT_SAVE_INTERVAL)
    {
        Checkpoint_save(map);
        map->lastCheckpointGeneration = map->generation;
    }
}
