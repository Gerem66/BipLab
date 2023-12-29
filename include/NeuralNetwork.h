#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <SDL.h>
#include <SDL2_framerate.h>
#include <SDL2_gfxPrimitives.h>

typedef struct NeuralLayer NeuralLayer;
typedef struct NeuralNetwork NeuralNetwork;

#include "cell.h"
#include "utils.h"

struct NeuralLayer {
    int neuronCount;
    int nextLayerNeuronCount;
    double *weights;
    double *outputs;
};

struct NeuralNetwork {
    int *topology;
    int topologySize;
    int weightCount;
    NeuralLayer **layers;
};

NeuralNetwork* createNeuralNetwork(int *topology, int topologySize);
void processInputs(NeuralNetwork *nn, double *inputs, double *outputs);
void mutateNeuralNetwork(NeuralNetwork *nn, NeuralNetwork *parent, double mutationRate, float mutationProbability);
void setRandomWeights(NeuralNetwork *nn, double minValue, double maxValue);
void freeNeuralNetwork(NeuralNetwork *nn);
void NeuralNetwork_Render(Cell *cell, SDL_Renderer *renderer, int index, int x, int y, int w, int h);

#endif // NEURALNETWORK_H
