#include "../../../include/core/game.h"
#include <limits.h>

void Game_reset(Map *map, bool fullReset)
{
    Cell *bestParents[MEM_CELL_COUNT];
    int parentCount = 0;

    if (fullReset)
    {
        // Use the best cell ever for full reset
        bestParents[0] = map->bestCellEver;
        parentCount = 1;
    }
    else
    {
        // Create array of all cells with their scores
        typedef struct {
            Cell *cell;
            int score;
        } CellScore;

        CellScore cellScores[MEM_CELL_COUNT];
        int validCellCount = 0;

        // Collect all valid cells
        for (int i = 0; i < map->cellCount; ++i) {
            if (map->cells[i] != NULL) {
                cellScores[validCellCount].cell = map->cells[i];
                cellScores[validCellCount].score = map->cells[i]->score;
                validCellCount++;
            }
        }

        // Sort cells by score (descending)
        for (int i = 0; i < validCellCount - 1; i++) {
            for (int j = 0; j < validCellCount - i - 1; j++) {
                if (cellScores[j].score < cellScores[j + 1].score) {
                    CellScore temp = cellScores[j];
                    cellScores[j] = cellScores[j + 1];
                    cellScores[j + 1] = temp;
                }
            }
        }

        // Select top X% as parents (minimum 1, maximum 10)
        parentCount = (int)(validCellCount * EVOLUTION_PARENT_SELECTION_RATIO);
        if (parentCount < 1) parentCount = 1;

        for (int i = 0; i < parentCount; i++) {
            bestParents[i] = cellScores[i].cell;
        }

        // Update evolution metrics and adapt mutation parameters BEFORE adding graph point
        Evolution_CalculateMetrics(map, &map->evolutionMetrics);
        Evolution_AdaptMutationParams(&map->evolutionMetrics, &map->mutationParams);

        // Add synchronized point to all graph curves
        Graph_AddPoint(&map->graphData, map);
    }

    // Reset & mutate cells using selected parents
    int revived = 0;
    while (revived < GAME_START_CELL_COUNT)
    {
        int targetIndex = -1;
        int worstScore = INT_MAX;

        // Find worst performing cell to replace
        for (int i = 0; i < map->cellCount; ++i)
        {
            if (map->cells[i] == NULL)
                continue;

            if (!map->cells[i]->isAlive && map->cells[i]->score < worstScore)
            {
                targetIndex = i;
                worstScore = map->cells[i]->score;
            }
        }
        if (targetIndex == -1)
        {
            fprintf(stderr, "Error while reviving cells !\n");
            return;
        }

        // Select a random parent from the top 20%
        Cell *selectedParent = bestParents[revived % parentCount];

        NeuralNetwork *newNN = NeuralNetwork_Copy(selectedParent->nn);
        if (newNN == NULL)
        {
            fprintf(stderr, "Failed to copy NeuralNetwork !\n");
            return;
        }

        Cell_reset(map->cells[targetIndex]);
        freeNeuralNetwork(map->cells[targetIndex]->nn);
        map->cells[targetIndex]->nn = newNN;

        // Use dynamic mutation parameters
        Cell_mutate(
            map->cells[targetIndex],
            map->mutationParams.resetMutationRate,
            map->mutationParams.resetMutationProb
        );

        revived++;
    }

    // Reset foods state
    for (int i = 0; i < GAME_START_FOOD_COUNT; ++i)
        Food_reset(map->foods[i], map);

    // Reset walls state
    for (int i = 0; i < GAME_START_WALL_COUNT; ++i)
        Wall_reset(map->walls[i], map);

    // Save frames of the ending generation
    map->previousGenFrames = map->frames;

    // Calculate GPS (Generations Per Second)
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
