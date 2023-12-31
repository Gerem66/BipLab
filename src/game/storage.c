#include "game.h"

bool Game_save(Map *map, char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erreur en ouvrant le fichier");
        return false;
    }

    // Save map time and generation
    fprintf(file, "%ld %d\n", map->startTime, map->generation);

    // Save the topology
    NeuralNetwork *nn = map->cells[map->currentBestCellIndex]->nn;
    fprintf(file, "%d\n", nn->topologySize);
    for (int i = 0; i < nn->topologySize; i++) {
        fprintf(file, "%d ", nn->topology[i]);
    }
    fprintf(file, "\n");

    // Save the weights
    for (int i = 0; i < nn->topologySize - 1; i++) {
        for (int j = 0; j < nn->layers[i]->neuronCount * nn->layers[i]->nextLayerNeuronCount; j++) {
            fprintf(file, "%.10lf ", nn->layers[i]->weights[j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return true;
}

NeuralNetwork* Game_load(Map *map, char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return NULL;
    }

    // Load map time and generation
    fscanf(file, "%ld %d", &map->startTime, &map->generation);

    // Load the topology
    int topologySize;
    fscanf(file, "%d", &topologySize);
    int *topology = (int *)malloc(topologySize * sizeof(int));
    for (int i = 0; i < topologySize; i++) {
        fscanf(file, "%d", &topology[i]);
    }

    // Create the neural network
    NeuralNetwork *nn = createNeuralNetwork(topology, topologySize);
    free(topology);

    // Load the weights
    for (int i = 0; i < topologySize - 1; i++) {
        for (int j = 0; j < nn->layers[i]->neuronCount * nn->layers[i]->nextLayerNeuronCount; j++) {
            fscanf(file, "%lf", &nn->layers[i]->weights[j]);
        }
    }

    fclose(file);
    return nn;
}
