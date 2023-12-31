#include "game.h"

bool saveNeuralNetwork(NeuralNetwork *nn, char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erreur en ouvrant le fichier");
        return false;
    }

    // Sauvegarder la topologie
    fprintf(file, "%d\n", nn->topologySize);
    for (int i = 0; i < nn->topologySize; i++) {
        fprintf(file, "%d ", nn->topology[i]);
    }
    fprintf(file, "\n");

    // Sauvegarder les poids
    for (int i = 0; i < nn->topologySize - 1; i++) {
        for (int j = 0; j < nn->layers[i]->neuronCount * nn->layers[i]->nextLayerNeuronCount; j++) {
            fprintf(file, "%.10lf ", nn->layers[i]->weights[j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return true;
}

NeuralNetwork* loadNeuralNetwork(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return NULL;
    }

    int topologySize;
    fscanf(file, "%d", &topologySize);
    int *topology = (int *)malloc(topologySize * sizeof(int));

    for (int i = 0; i < topologySize; i++) {
        fscanf(file, "%d", &topology[i]);
    }

    NeuralNetwork *nn = createNeuralNetwork(topology, topologySize);
    free(topology);

    for (int i = 0; i < topologySize - 1; i++) {
        for (int j = 0; j < nn->layers[i]->neuronCount * nn->layers[i]->nextLayerNeuronCount; j++) {
            fscanf(file, "%lf", &nn->layers[i]->weights[j]);
        }
    }

    fclose(file);
    return nn;
}
