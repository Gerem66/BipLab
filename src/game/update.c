#include <math.h>

#ifdef HAVE_OPENMP
#include <omp.h>
#endif

#include "game.h"

void Game_update(Map *map)
{
    if (!map->isRunning)
        return;

    map->frames++;

    // Compter les UPS (Updates Per Second)
    static time_t lastUPSTime = 0;
    static int updateCount = 0;
    updateCount++;

    time_t currentUPSTime = time(NULL);
    if (currentUPSTime != lastUPSTime && lastUPSTime != 0) {
        map->currentUPS = updateCount;
        updateCount = 0;
    }
    lastUPSTime = currentUPSTime;

    // Update cells - Parallelized with OpenMP
#ifdef HAVE_OPENMP
    #pragma omp parallel for
#endif
    for (int i = 0; i < map->cellCount; ++i) {
        if (map->cells[i] != NULL) {
            Cell_update(map->cells[i], map);
        }
    }

    // Check generation
    bool allDead = true;
    for (int i = 0; i < map->cellCount; ++i)
    {
        if (map->cells[i] != NULL && map->cells[i]->isAlive)
        {
            allDead = false;
            break;
        }
    }

    // Next generation
    if (allDead)
        Game_reset(map, false);

    // Update best cell
    int bestCellIndex = 0;
    for (int i = 0; i < map->cellCount; ++i)
    {
        if (map->cells[i] == NULL || map->cells[bestCellIndex] == NULL)
            continue;
        if (map->cells[i]->score > map->cells[bestCellIndex]->score)
            bestCellIndex = i;
    }
    map->currentBestCellIndex = bestCellIndex;

    if (map->cells[bestCellIndex]->score > map->bestCellEver->score)
    {
        NeuralNetwork *bestNN = NeuralNetwork_Copy(map->cells[bestCellIndex]->nn);
        if (bestNN != NULL)
        {
            freeNeuralNetwork(map->bestCellEver->nn);
            map->bestCellEver->nn = bestNN;
        }

        map->bestCellEver->score = map->cells[bestCellIndex]->score;
        map->bestCellEver->generation = map->generation;
    }

    // Update oldest cell
    Cell *oldestCell = map->cells[0];
    for (int i = 0; i < map->cellCount; ++i)
        if (map->cells[i] != NULL && map->cells[i]->generation > oldestCell->generation)
            oldestCell = map->cells[i];
    if (oldestCell->generation > map->maxGeneration)
        map->maxGeneration = oldestCell->generation;

    // Check graph timeout
    Graph_CheckTimeout(&map->graphData, map);
}
