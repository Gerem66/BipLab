#include "NeuralNetwork.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>


NeuralLayer* createNeuralLayer(int neuronCount, int nextLayerNeuronCount) {
    NeuralLayer *layer = (NeuralLayer *)malloc(sizeof(NeuralLayer));
    layer->neuronCount = neuronCount;
    layer->nextLayerNeuronCount = nextLayerNeuronCount;
    layer->weights = (double *)malloc(neuronCount * nextLayerNeuronCount * sizeof(double));
    layer->outputs = (double *)malloc(nextLayerNeuronCount * sizeof(double));
    return layer;
}

void freeNeuralLayer(NeuralLayer *layer) {
    free(layer->weights);
    free(layer->outputs);
    free(layer);
}

double randomWeight(double minValue, double maxValue) {
    return minValue + (double)rand() / ((double)RAND_MAX / (maxValue - minValue));
}

NeuralNetwork* createNeuralNetwork(int *topology, int topologySize) {
    NeuralNetwork *nn = (NeuralNetwork *)malloc(sizeof(NeuralNetwork));
    nn->topology = (int *)malloc(topologySize * sizeof(int));
    nn->topologySize = topologySize;
    nn->weightCount = 0;
    nn->layers = (NeuralLayer **)malloc((topologySize - 1) * sizeof(NeuralLayer *));

    for (int i = 0; i < topologySize - 1; i++) {
        nn->topology[i] = topology[i];
        nn->layers[i] = createNeuralLayer(topology[i], topology[i + 1]);
        nn->weightCount += (topology[i] + 1) * topology[i + 1]; // +1 pour le biais
    }

    return nn;
}

void processInputs(NeuralNetwork *nn, double *inputs, double *outputs) {
    double *currentOutputs = inputs;
    for (int i = 0; i < nn->topologySize - 1; i++) {
        NeuralLayer *layer = nn->layers[i];
        for (int j = 0; j < layer->nextLayerNeuronCount; j++) {
            layer->outputs[j] = 0.0;
            for (int k = 0; k < layer->neuronCount; k++) {
                layer->outputs[j] += currentOutputs[k] * layer->weights[k * layer->nextLayerNeuronCount + j];
            }
            layer->outputs[j] = 1.0 / (1.0 + exp(-layer->outputs[j]));
        }
        currentOutputs = layer->outputs;
    }
    for (int i = 0; i < nn->layers[nn->topologySize - 2]->nextLayerNeuronCount; i++) {
        outputs[i] = currentOutputs[i];
    }
}

void mutateNeuralNetwork(NeuralNetwork *nn, NeuralNetwork *parent, double mutationRate, float mutationProbability)
{
    for (int i = 0; i < nn->topologySize - 1; i++) {
        NeuralLayer *layer = nn->layers[i];
        NeuralLayer *parentLayer = parent->layers[i];
        for (int j = 0; j < layer->neuronCount * layer->nextLayerNeuronCount; j++) {
            if (rand() / (double)RAND_MAX < mutationProbability)
            {
                layer->weights[j] = randomWeight(-1, 1);
            }
            else
            {
                double delta = parentLayer->weights[j] - layer->weights[j];
                double mutation = delta * mutationRate;
                layer->weights[j] += mutation;
            }
        }
    }
}

void setRandomWeights(NeuralNetwork *nn, double minValue, double maxValue) {
    for (int i = 0; i < nn->topologySize - 1; i++) {
        NeuralLayer *layer = nn->layers[i];
        for (int j = 0; j < layer->neuronCount * layer->nextLayerNeuronCount; j++) {
            layer->weights[j] = randomWeight(minValue, maxValue);
        }
    }
}

void freeNeuralNetwork(NeuralNetwork *nn) {
    for (int i = 0; i < nn->topologySize - 1; i++) {
        freeNeuralLayer(nn->layers[i]);
    }
    free(nn->layers);
    free(nn->topology);
    free(nn);
}
