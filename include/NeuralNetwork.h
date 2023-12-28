#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

typedef struct NeuralLayer NeuralLayer;
typedef struct NeuralNetwork NeuralNetwork;

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

#endif // NEURALNETWORK_H
