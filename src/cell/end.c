#include "cell.h"

void Cell_reset(Cell *cell)
{
    cell->isAlive = true;
    cell->score = 0;
    cell->health = cell->healthInit;
    cell->frame = 0;
    cell->generation = 1;

    cell->position.x = cell->positionInit.x;
    cell->position.y = cell->positionInit.y;
    cell->speed = 0.0f;
    cell->angle = 0.0f;

    cell->goingUp = false;
    cell->goingDown = false;
    cell->goingLeft = false;
    cell->goingRight = false;
}

void Cell_destroy(Cell *cell)
{
    freeNeuralNetwork(cell->nn);
    free(cell);
}
