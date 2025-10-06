#include "../../include/cell.h"

Cell *Cell_create(SDL_Texture *sprite, int x, int y, bool isAI)
{
    Cell *cell = malloc(sizeof(Cell));
    if (cell == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for Cell !\n");
        return NULL;
    }

    cell->isAI = isAI;
    cell->positionInit.x = x;
    cell->positionInit.y = y;
    cell->healthInit = 30;
    cell->healthMax = 100;
    cell->framePerHealth = 20;
    cell->birthCostMax = 40;

    cell->speedMax = 3.0f;
    cell->velocity = 0.4f;
    cell->angleVelocity = 4.0f;

    cell->radius = 10;
    cell->hitbox.x = x - cell->radius;
    cell->hitbox.y = y - cell->radius;
    cell->hitbox.w = cell->radius * 2;
    cell->hitbox.h = cell->radius * 2;
    
    // Set sprite
    cell->sprite = sprite;

    // Init rays from -PI to PI
    float demiAngle = PI / 4.0f;
    for (int i = 0; i < 7; i++)
    {
        cell->rays[i].angle = -demiAngle + i * demiAngle / 3;
        cell->rays[i].distance = 0.0f;
        cell->rays[i].distanceMax = 600.0f;
        cell->raysWall[i].angle = -demiAngle + i * demiAngle / 3;
        cell->raysWall[i].distance = 0.0f;
        cell->raysWall[i].distanceMax = 600.0f;
    }

    Cell_reset(cell);

    // Create NeuralNetwork
    int topologySize = NEURAL_NETWORK_INIT_TOPOLOGY_SIZE;
    int topology[] = NEURAL_NETWORK_INIT_TOPOLOGY;
    cell->nn = createNeuralNetwork(topology, topologySize);
    setRandomWeights(cell->nn, -1, 1);

    return cell;
}

void Cell_GiveBirth(Cell *cell, Map *map)
{
    // Priorize lower scores
    int index = -1;
    int minValue = -1;
    for (int i = 1; i < CELL_COUNT; i++)
    {
        if (map->cells[i] == NULL || map->cells[i]->isAlive)
            continue;

        if (map->cells[i]->score < minValue || minValue == -1)
        {
            index = i;
            minValue = map->cells[i]->score;
        }
    }

    if (index == -1 && map->cellCount < CELL_COUNT)
    {
        index = map->cellCount++;
    }

    if (index == -1)
    {
        printf("No more space for new cells !\n");
        return;
    }

    // Load cell sprite if sprite rendering is enabled
    SDL_Texture *sprite = NULL;
    if (CELL_USE_SPRITE) {
        char spritePath[256];
        snprintf(spritePath, sizeof(spritePath), "ressources/bipboup/normal/skin.png");
        sprite = IMG_LoadTexture(map->renderer, spritePath);
    }

    Cell *newCell = Cell_create(sprite, cell->positionInit.x, cell->positionInit.y, true);
    newCell->position.x = cell->position.x;
    newCell->position.y = cell->position.y;
    newCell->generation = cell->generation + 1;

    // Copy NeuralNetwork and mutate
    NeuralNetwork *newNN = NeuralNetwork_Copy(cell->nn);
    if (newNN == NULL)
    {
        fprintf(stderr, "Failed to copy NeuralNetwork !\n");
        return;
    }

    freeNeuralNetwork(newCell->nn);
    newCell->nn = newNN;
    Cell_mutate(newCell, NEURAL_NETWORK_CHILD_MUTATION_RATE, NEURAL_NETWORK_CHILD_MUTATION_PROB, NEURAL_NETWORK_TOPOLOGY_MUTATION_PROBABILITY);

    if (map->cells[index] != NULL)
        Cell_destroy(map->cells[index]);
    map->cells[index] = newCell;
}
