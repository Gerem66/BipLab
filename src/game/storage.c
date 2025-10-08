#include "game.h"

bool Game_exists(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return false;
    }
    fclose(file);
    return true;
}

bool Game_save(Map *map, char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erreur en ouvrant le fichier");
        return false;
    }

    // Save map time and generation
    time_t duration = time(NULL) - map->startTime;
    fprintf(file, "%ld %d\n", duration, map->generation);

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

    // Save the biases
    for (int i = 0; i < nn->topologySize - 1; i++) {
        for (int j = 0; j < nn->layers[i]->nextLayerNeuronCount; j++) {
            fprintf(file, "%.10lf ", nn->layers[i]->biases[j]);
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
    time_t duration;
    fscanf(file, "%ld %d", &duration, &map->generation);
    map->startTime = time(NULL) - duration;

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

    // Load the biases
    for (int i = 0; i < topologySize - 1; i++) {
        for (int j = 0; j < nn->layers[i]->nextLayerNeuronCount; j++) {
            fscanf(file, "%lf", &nn->layers[i]->biases[j]);
        }
    }

    fclose(file);
    return nn;
}
