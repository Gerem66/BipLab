#include "../../include/ai/neuralNetwork.h"
#include "../../include/system/performance.h"

NeuralLayer *createNeuralLayer(int neuronCount, int nextLayerNeuronCount)
{
    NeuralLayer *layer = (NeuralLayer *)malloc(sizeof(NeuralLayer));
    if (layer == NULL)
    {
        return NULL;
    }

    layer->neuronCount = neuronCount;
    layer->nextLayerNeuronCount = nextLayerNeuronCount;
    layer->weights = (double *)malloc(neuronCount * nextLayerNeuronCount * sizeof(double));
    layer->biases  = (double *)malloc(nextLayerNeuronCount * sizeof(double));
    layer->outputs = (double *)malloc(nextLayerNeuronCount * sizeof(double));
    return layer;
}

void freeNeuralLayer(NeuralLayer *layer)
{
    free(layer->weights);
    free(layer->biases);
    free(layer->outputs);
    free(layer);
}

void setRandomWeights(NeuralNetwork *nn, double minValue, double maxValue)
{
    for (int i = 0; i < nn->topologySize - 1; i++)
    {
        NeuralLayer *layer = nn->layers[i];

        // Initialize weights with random values
        for (int j = 0; j < layer->neuronCount * layer->nextLayerNeuronCount; j++)
        {
            layer->weights[j] = drand(minValue, maxValue);
        }

        // Initialize biases with smaller random values
        for (int j = 0; j < layer->nextLayerNeuronCount; j++)
        {
            layer->biases[j] = drand(minValue * 0.5, maxValue * 0.5);
        }
    }
}

NeuralNetwork *createNeuralNetwork(int *topology, int topologySize)
{
    NeuralNetwork *nn = (NeuralNetwork *)malloc(sizeof(NeuralNetwork));
    if (nn == NULL)
    {
        return NULL;
    }
    nn->topology = (int *)malloc(topologySize * sizeof(int));
    if (nn->topology == NULL)
    {
        free(nn);
        return NULL;
    }
    nn->topologySize = topologySize;
    nn->layers = (NeuralLayer **)malloc((topologySize - 1) * sizeof(NeuralLayer *));
    if (nn->layers == NULL)
    {
        free(nn->topology);
        free(nn);
        return NULL;
    }

    for (int i = 0; i < topologySize - 1; i++)
    {
        nn->topology[i] = topology[i];
        nn->layers[i] = createNeuralLayer(topology[i], topology[i + 1]);
        if (nn->layers[i] == NULL)
        {
            free(nn->topology);
            free(nn->layers);
            free(nn);
            return NULL;
        }
    }
    nn->topology[topologySize - 1] = topology[topologySize - 1];

    return nn;
}

NeuralNetwork *NeuralNetwork_Copy(NeuralNetwork *parent)
{
    NeuralNetwork *newNN = createNeuralNetwork(parent->topology, parent->topologySize);
    if (newNN == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < newNN->topologySize - 1; i++)
    {
        NeuralLayer *newLayer = newNN->layers[i];
        NeuralLayer *parentLayer = parent->layers[i];

        // Copy the weights
        for (int j = 0; j < newLayer->neuronCount * newLayer->nextLayerNeuronCount; j++)
        {
            newLayer->weights[j] = parentLayer->weights[j];
        }

        // Copy the biases
        for (int j = 0; j < newLayer->nextLayerNeuronCount; j++)
        {
            newLayer->biases[j] = parentLayer->biases[j];
        }
    }

    return newNN;
}

void processInputs(NeuralNetwork *nn, double *inputs, double *outputs)
{
    PERF_MEASURE(PERF_NEURAL_NETWORK) {
        double *currentOutputs = inputs;
        for (int i = 0; i < nn->topologySize - 1; i++)
        {
            NeuralLayer *layer = nn->layers[i];
            for (int j = 0; j < layer->nextLayerNeuronCount; j++)
            {
                // Start with the bias
                layer->outputs[j] = layer->biases[j];

                for (int k = 0; k < layer->neuronCount; k++)
                {
                    layer->outputs[j] += currentOutputs[k] * layer->weights[k * layer->nextLayerNeuronCount + j];
                }
                layer->outputs[j] = tanh(layer->outputs[j]);
            }
            currentOutputs = layer->outputs;
        }
        for (int i = 0; i < nn->layers[nn->topologySize - 2]->nextLayerNeuronCount; i++)
        {
            outputs[i] = currentOutputs[i];
        }
    } // PERF_MEASURE
}

void mutate_NeuralNetwork_Weights(NeuralNetwork *nn, double mutationRate, float mutationProbability)
{
    PERF_MEASURE(PERF_MUTATION) {
        for (int i = 0; i < nn->topologySize - 1; i++)
        {
            NeuralLayer *layer = nn->layers[i];

            // Weight mutation
            for (int j = 0; j < layer->neuronCount * layer->nextLayerNeuronCount; j++)
            {
                if (rand() / (double)RAND_MAX < mutationProbability)
                {
                    layer->weights[j] += drand(-mutationRate, mutationRate);
                }
            }

            // Bias mutation
            for (int j = 0; j < layer->nextLayerNeuronCount; j++)
            {
                if (rand() / (double)RAND_MAX < mutationProbability)
                {
                    layer->biases[j] += drand(-mutationRate, mutationRate);
                }
            }
        }
    } // PERF_MEASURE
}

/**
 * @brief Can mutate the topology of the neural network:
 * - Add a neuron to a random hidden layer (not input/output)
 * - Remove a neuron from a random hidden layer (not input/output)
 * Note: Layer addition/removal is disabled as it's complex to implement correctly
 *
 * @param nn
 * @param maxNeurons
 * @param maxLayers
 * @param mutationProbability
 */
void mutate_NeuralNetwork_Topology(NeuralNetwork *nn, int maxNeurons, int maxLayers, float mutationProbability)
{
    (void)maxLayers; // Suppress unused parameter warning

    if (rand() / (double)RAND_MAX >= mutationProbability) {
        return;
    }

    // Only allow neuron addition/removal in hidden layers (not input/output)
    if (nn->topologySize < 3) {
        return; // Need at least input + hidden + output
    }

    int mutationType = rand() % 2; // 0 = add neuron, 1 = remove neuron

    if (mutationType == 0)
    {
        // Add a neuron to a random HIDDEN layer (exclude input and output)
        int hiddenLayerCount = nn->topologySize - 2;
        if (hiddenLayerCount <= 0) return;

        int hiddenLayerIndex = rand() % hiddenLayerCount; // 0 to hiddenLayerCount-1
        int layerIndex = hiddenLayerIndex + 1; // +1 to skip input layer

        NeuralLayer *currentLayer = nn->layers[layerIndex];
        int currentNeurons = currentLayer->neuronCount;
        int nextLayerNeurons = currentLayer->nextLayerNeuronCount;

        if (currentNeurons >= maxNeurons) {
            return; // Already at max capacity
        }

        // Create new layer with one more neuron
        NeuralLayer *newLayer = createNeuralLayer(currentNeurons + 1, nextLayerNeurons);
        if (newLayer == NULL) {
            return;
        }

        // Copy existing weights from current layer to new layer
        // Format: weights[from_neuron * next_layer_size + to_neuron]
        for (int from = 0; from < currentNeurons; from++) {
            for (int to = 0; to < nextLayerNeurons; to++) {
                int oldIndex = from * nextLayerNeurons + to;
                int newIndex = from * nextLayerNeurons + to;
                newLayer->weights[newIndex] = currentLayer->weights[oldIndex];
            }
        }

        // Initialize weights for the new neuron (last neuron)
        for (int to = 0; to < nextLayerNeurons; to++) {
            int newIndex = currentNeurons * nextLayerNeurons + to;
            newLayer->weights[newIndex] = drand(-0.5, 0.5);
        }

        // Copy existing biases
        for (int i = 0; i < nextLayerNeurons; i++) {
            newLayer->biases[i] = currentLayer->biases[i];
        }

        // Update topology
        nn->topology[layerIndex] = currentNeurons + 1;

        // Update previous layer if it exists to connect to the new neuron
        if (layerIndex > 0) {
            NeuralLayer *prevLayer = nn->layers[layerIndex - 1];
            int prevNeurons = prevLayer->neuronCount;
            int prevNextLayerNeurons = prevLayer->nextLayerNeuronCount;

            // Create new previous layer with updated connections
            NeuralLayer *newPrevLayer = createNeuralLayer(prevNeurons, currentNeurons + 1);
            if (newPrevLayer == NULL) {
                freeNeuralLayer(newLayer);
                return;
            }

            // Copy existing weights and add new connections to the new neuron
            for (int from = 0; from < prevNeurons; from++) {
                for (int to = 0; to < currentNeurons; to++) {
                    int oldIndex = from * prevNextLayerNeurons + to;
                    int newIndex = from * (currentNeurons + 1) + to;
                    newPrevLayer->weights[newIndex] = prevLayer->weights[oldIndex];
                }
                // Add connection to new neuron
                int newNeuronIndex = from * (currentNeurons + 1) + currentNeurons;
                newPrevLayer->weights[newNeuronIndex] = drand(-0.5, 0.5);
            }

            // Copy biases and add bias for new neuron
            for (int i = 0; i < currentNeurons; i++) {
                newPrevLayer->biases[i] = prevLayer->biases[i];
            }
            newPrevLayer->biases[currentNeurons] = drand(-0.5, 0.5);

            // Replace previous layer
            freeNeuralLayer(nn->layers[layerIndex - 1]);
            nn->layers[layerIndex - 1] = newPrevLayer;
        }

        // Replace current layer
        freeNeuralLayer(nn->layers[layerIndex]);
        nn->layers[layerIndex] = newLayer;
    }
    else if (mutationType == 1)
    {
        // Remove a neuron from a random HIDDEN layer
        int hiddenLayerCount = nn->topologySize - 2;
        if (hiddenLayerCount <= 0) return;

        int hiddenLayerIndex = rand() % hiddenLayerCount;
        int layerIndex = hiddenLayerIndex + 1; // +1 to skip input layer

        NeuralLayer *currentLayer = nn->layers[layerIndex];
        int currentNeurons = currentLayer->neuronCount;
        int nextLayerNeurons = currentLayer->nextLayerNeuronCount;

        if (currentNeurons <= 1) {
            return; // Can't remove the last neuron
        }

        int neuronToRemove = rand() % currentNeurons;

        // Create new layer with one less neuron
        NeuralLayer *newLayer = createNeuralLayer(currentNeurons - 1, nextLayerNeurons);
        if (newLayer == NULL) {
            return;
        }

        // Copy weights, skipping the removed neuron
        int newFrom = 0;
        for (int from = 0; from < currentNeurons; from++) {
            if (from == neuronToRemove) continue;

            for (int to = 0; to < nextLayerNeurons; to++) {
                int oldIndex = from * nextLayerNeurons + to;
                int newIndex = newFrom * nextLayerNeurons + to;
                newLayer->weights[newIndex] = currentLayer->weights[oldIndex];
            }
            newFrom++;
        }

        // Copy biases
        for (int i = 0; i < nextLayerNeurons; i++) {
            newLayer->biases[i] = currentLayer->biases[i];
        }

        // Update topology
        nn->topology[layerIndex] = currentNeurons - 1;

        // Update previous layer if it exists
        if (layerIndex > 0) {
            NeuralLayer *prevLayer = nn->layers[layerIndex - 1];
            int prevNeurons = prevLayer->neuronCount;
            int prevNextLayerNeurons = prevLayer->nextLayerNeuronCount;

            // Create new previous layer with updated connections
            NeuralLayer *newPrevLayer = createNeuralLayer(prevNeurons, currentNeurons - 1);
            if (newPrevLayer == NULL) {
                freeNeuralLayer(newLayer);
                return;
            }

            // Copy weights, skipping connections to the removed neuron
            for (int from = 0; from < prevNeurons; from++) {
                int newTo = 0;
                for (int to = 0; to < prevNextLayerNeurons; to++) {
                    if (to == neuronToRemove) continue;

                    int oldIndex = from * prevNextLayerNeurons + to;
                    int newIndex = from * (currentNeurons - 1) + newTo;
                    newPrevLayer->weights[newIndex] = prevLayer->weights[oldIndex];
                    newTo++;
                }
            }

            // Copy biases, skipping the removed neuron's bias
            int newBiasIndex = 0;
            for (int i = 0; i < prevNextLayerNeurons; i++) {
                if (i == neuronToRemove) continue;
                newPrevLayer->biases[newBiasIndex] = prevLayer->biases[i];
                newBiasIndex++;
            }

            // Replace previous layer
            freeNeuralLayer(nn->layers[layerIndex - 1]);
            nn->layers[layerIndex - 1] = newPrevLayer;
        }

        // Replace current layer
        freeNeuralLayer(nn->layers[layerIndex]);
        nn->layers[layerIndex] = newLayer;
    }
}

void freeNeuralNetwork(NeuralNetwork *nn)
{
    for (int i = 0; i < nn->topologySize - 1; i++)
    {
        freeNeuralLayer(nn->layers[i]);
    }
    free(nn->layers);
    free(nn->topology);
    free(nn);
}
