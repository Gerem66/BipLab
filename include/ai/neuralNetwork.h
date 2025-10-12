#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_framerate.h>
#include <SDL2/SDL2_gfxPrimitives.h>

typedef struct NeuralLayer NeuralLayer;
typedef struct NeuralNetwork NeuralNetwork;

#include "../entities/cell.h"
#include "../core/utils.h"

struct NeuralLayer {
    int neuronCount;
    int nextLayerNeuronCount;
    double *weights;
    double *biases;
    double *outputs;
};

struct NeuralNetwork {
    int *topology;
    int topologySize;
    NeuralLayer **layers;
};

NeuralNetwork *createNeuralNetwork(int *topology, int topologySize);
NeuralNetwork *NeuralNetwork_Copy(NeuralNetwork *parent);
void processInputs(NeuralNetwork *nn, double *inputs, double *outputs);
void mutate_NeuralNetwork_Weights(NeuralNetwork *nn, double mutationRate, float mutationProbability);
void mutate_NeuralNetwork_Topology(NeuralNetwork *nn, int maxNeurons, int maxLayers, float mutationProbability);
void setRandomWeights(NeuralNetwork *nn, double minValue, double maxValue);
void freeNeuralNetwork(NeuralNetwork *nn);

#endif // NEURALNETWORK_H
