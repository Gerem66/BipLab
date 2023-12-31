#include "cell.h"

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
    cell->healthInit = 20;
    cell->healthMax = 50;
    cell->framePerHealth = 30;
    cell->birthCostMax = 15;

    cell->speedMin = -2.0f;
    cell->speedMax = 3.0f;
    cell->velocity = 0.4f;
    cell->angleVelocity = 4.0f;

    cell->radius = 10;
    cell->color = COLOR_BLUE;
    if (!isAI)
        cell->color = COLOR_GREEN;
    cell->sprite = sprite;

    // Init rays from -PI to PI
    for (int i = 0; i < 7; i++)
    {
        cell->rays[i].angle = -PI + i * PI / 3;
        cell->rays[i].distance = 0.0f;
        cell->rays[i].distanceMax = 100.0f;
        cell->raysWall[i].angle = -PI + i * PI / 3;
        cell->raysWall[i].distance = 0.0f;
        cell->raysWall[i].distanceMax = 100.0f;
    }

    Cell_reset(cell);

    // Create NeuralNetwork
    int topology[] = {15, 10, 8, 4};
    cell->nn = createNeuralNetwork(topology, 4);
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

    SDL_Texture *sprite = LoadSprite(map->renderer, "../ressources/cell.png");
    if (sprite == NULL)
    {
        printf("Erreur de création de la texture : %s", SDL_GetError());
        return;
    }

    Cell *newCell = Cell_create(sprite, cell->positionInit.x, cell->positionInit.y, true);
    newCell->position.x = cell->position.x;
    newCell->position.y = cell->position.y;
    newCell->generation = cell->generation + 1;
    Cell_mutate(newCell, cell, 1.0f, 0.05f);

    if (map->cells[index] != NULL)
        Cell_destroy(map->cells[index]);
    map->cells[index] = newCell;
}
