#include "neuralNetwork.h"

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
    layer->outputs = (double *)malloc(nextLayerNeuronCount * sizeof(double));
    return layer;
}

void freeNeuralLayer(NeuralLayer *layer)
{
    free(layer->weights);
    free(layer->outputs);
    free(layer);
}

void setRandomWeights(NeuralNetwork *nn, double minValue, double maxValue)
{
    for (int i = 0; i < nn->topologySize - 1; i++)
    {
        NeuralLayer *layer = nn->layers[i];
        for (int j = 0; j < layer->neuronCount * layer->nextLayerNeuronCount; j++)
        {
            layer->weights[j] = drand(minValue, maxValue);
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
    }

    return newNN;
}

void processInputs(NeuralNetwork *nn, double *inputs, double *outputs)
{
    double *currentOutputs = inputs;
    for (int i = 0; i < nn->topologySize - 1; i++)
    {
        NeuralLayer *layer = nn->layers[i];
        for (int j = 0; j < layer->nextLayerNeuronCount; j++)
        {
            layer->outputs[j] = 0.0;
            for (int k = 0; k < layer->neuronCount; k++)
            {
                layer->outputs[j] += currentOutputs[k] * layer->weights[k * layer->nextLayerNeuronCount + j];
            }
            layer->outputs[j] = 1.0 / (1.0 + exp(-layer->outputs[j]));
        }
        currentOutputs = layer->outputs;
    }
    for (int i = 0; i < nn->layers[nn->topologySize - 2]->nextLayerNeuronCount; i++)
    {
        outputs[i] = currentOutputs[i];
    }
}

void mutate_NeuralNetwork_Weights(NeuralNetwork *nn, double mutationRate, float mutationProbability)
{
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
    }
}

/**
 * @brief Can mutate the topology of the neural network:
 * - Add a neuron to a random layer
 * - Remove a neuron from a random layer
 * - Add a layer
 * - Remove a layer
 *
 * @param nn
 * @param maxNeurons
 * @param maxLayers
 * @param mutationProbability
 */
void mutate_NeuralNetwork_Topology(NeuralNetwork *nn, int maxNeurons, int maxLayers, float mutationProbability)
{
    if (rand() / (double)RAND_MAX >= mutationProbability) {
        return;
    }

    int mutationType = rand() % 6;

    if (mutationType == 0 || mutationType == 1)
    {
        // Add a neuron to a random hidden layer
        int layerIndex = rand() % (nn->topologySize - 1); // Exclude the output layer
        int neuronCount = nn->layers[layerIndex]->neuronCount;
        int nextLayerNeuronCount = nn->layers[layerIndex]->nextLayerNeuronCount;

        if (neuronCount < maxNeurons)
        {
            NeuralLayer *newLayer = createNeuralLayer(neuronCount + 1, nextLayerNeuronCount);
            if (newLayer == NULL)
            {
                return;
            }

            // Copy the weights from the old layer
            for (int i = 0; i < neuronCount; i++)
            {
                newLayer->weights[i] = rand() / (double)RAND_MAX;
            }

            freeNeuralLayer(nn->layers[layerIndex]);
            nn->layers[layerIndex] = newLayer;
        }
    }
    else if (mutationType == 2 || mutationType == 3)
    {
        // Remove a neuron from a random hidden layer
        int layerIndex = rand() % (nn->topologySize - 1); // Exclude the output layer
        int neuronCount = nn->layers[layerIndex]->neuronCount;
        int nextLayerNeuronCount = nn->layers[layerIndex]->nextLayerNeuronCount;

        if (neuronCount > 1)
        {
            NeuralLayer *newLayer = createNeuralLayer(neuronCount - 1, nextLayerNeuronCount);
            if (newLayer == NULL)
            {
                return;
            }

            // Copy the weights from the old layer
            for (int i = 0; i < neuronCount - 1; i++)
            {
                newLayer->weights[i] = nn->layers[layerIndex]->weights[i];
            }

            freeNeuralLayer(nn->layers[layerIndex]);
            nn->layers[layerIndex] = newLayer;
        }
    }
    else if (mutationType == 4 && nn->topologySize < maxLayers)
    {
        return;

        // Choose a random position to insert the new layer (excluding input and output)
        int insertPosition = 1 + rand() % (nn->topologySize - 2);

        // Determine the number of neurons in the new layer
        int newLayerSize = nn->topology[insertPosition];

        // Create a new topology array
        int *newTopology = (int *)malloc((nn->topologySize + 1) * sizeof(int));
        if (newTopology == NULL)
        {
            return;
        }

        // Copy the topology, inserting the new layer
        for (int i = 0; i < insertPosition; i++)
        {
            newTopology[i] = nn->topology[i];
        }
        newTopology[insertPosition] = newLayerSize;
        for (int i = insertPosition; i < nn->topologySize; i++)
        {
            newTopology[i + 1] = nn->topology[i];
        }

        // Create new layers array
        NeuralLayer **newLayers = (NeuralLayer **)malloc(nn->topologySize * sizeof(NeuralLayer *));
        if (newLayers == NULL)
        {
            free(newTopology);
            return;
        }

        // Copy existing layers and create the new layer
        for (int i = 0; i < insertPosition; i++)
        {
            newLayers[i] = nn->layers[i];
        }
        newLayers[insertPosition] = createNeuralLayer(newLayerSize, nn->topology[insertPosition]);
        if (newLayers[insertPosition] == NULL)
        {
            free(newTopology);
            free(newLayers);
            return;
        }
        for (int i = insertPosition; i < nn->topologySize - 1; i++)
        {
            newLayers[i + 1] = nn->layers[i];
        }

        // Update the neural network
        free(nn->topology);
        free(nn->layers);
        nn->topology = newTopology;
        nn->layers = newLayers;
        nn->topologySize++;

        // Initialize weights for the new layer
        setRandomWeights(nn, -1.0, 1.0);
    }
    else if (mutationType == 5 && nn->topologySize > 3)
    {
        return;

        // Choose a random hidden layer to remove
        int removePosition = 1 + rand() % (nn->topologySize - 2);

        // Create a new topology array
        int *newTopology = (int *)malloc((nn->topologySize - 1) * sizeof(int));
        if (newTopology == NULL)
        {
            return;
        }

        // Copy the topology, skipping the removed layer
        for (int i = 0; i < removePosition; i++)
        {
            newTopology[i] = nn->topology[i];
        }
        for (int i = removePosition + 1; i < nn->topologySize; i++)
        {
            newTopology[i - 1] = nn->topology[i];
        }

        // Create new layers array
        NeuralLayer **newLayers = (NeuralLayer **)malloc((nn->topologySize - 2) * sizeof(NeuralLayer *));
        if (newLayers == NULL)
        {
            free(newTopology);
            return;
        }

        // Copy layers, skipping the removed one
        for (int i = 0; i < removePosition; i++)
        {
            newLayers[i] = nn->layers[i];
        }
        for (int i = removePosition + 1; i < nn->topologySize - 1; i++)
        {
            newLayers[i - 1] = nn->layers[i];
        }

        // Free the removed layer
        freeNeuralLayer(nn->layers[removePosition]);

        // Update the neural network
        free(nn->topology);
        free(nn->layers);
        nn->topology = newTopology;
        nn->layers = newLayers;
        nn->topologySize--;
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

// By ChatGPT
void NeuralNetwork_Render(Cell *cell, SDL_Renderer *renderer, int index, int x, int y, int w, int h)
{
    // Assurez-vous que cell et cell->nn ne sont pas NULL.
    if (cell == NULL || cell->nn == NULL)
    {
        return;
    }

    // Show index of cell
    char indexText[50];
    sprintf(indexText, "Best cell: %d, with score: %d", index, cell->score);
    SDL_Color color = {255, 255, 255, 255};
    stringRGBA(renderer, x, y, indexText, color.r, color.g, color.b, color.a);

    NeuralNetwork *nn = cell->nn;
    int layerCount = nn->topologySize - 1;

    // Calculez l'espacement entre les layers et les neurones.
    int layerSpacing = w / (layerCount + 2);
    int maxNeurons = 0;
    for (int i = 0; i < layerCount; i++)
    {
        if (nn->layers[i]->neuronCount > maxNeurons)
        {
            maxNeurons = nn->layers[i]->neuronCount;
        }
    }
    int neuronSpacing = h / (maxNeurons + 1);

    // Dessinez les layers et les neurones.
    for (int i = 0; i < layerCount; i++)
    {
        int layerX = x + (i + 1) * layerSpacing;
        NeuralLayer *layer = nn->layers[i];

        for (int j = 0; j < layer->neuronCount; j++)
        {
            int neuronY = y + (j + 1) * neuronSpacing;
            int nextLayerNeuronCount = (i < layerCount - 1) ? nn->layers[i + 1]->neuronCount : 0;

            // Dessinez les liaisons avec la couche suivante.
            if (i < layerCount - 1)
            {
                for (int k = 0; k < nextLayerNeuronCount; k++)
                {
                    int nextNeuronY = y + (k + 1) * neuronSpacing;
                    int neuronWeightIndex = j * nextLayerNeuronCount + k;
                    float weight = nn->layers[i]->weights[neuronWeightIndex];
                    int opacity = (int)(255.0f * fabs(weight) / 10.0f);
                    if (weight < 0)
                    {
                        SDL_SetRenderDrawColor(renderer, 255, 0, 0, opacity);
                        if (!cell->isAlive)
                            SDL_SetRenderDrawColor(renderer, 125, 125, 125, opacity);
                    }
                    else
                    {
                        SDL_SetRenderDrawColor(renderer, 0, 255, 0, opacity);
                    }
                    if (!cell->isAlive)
                    {
                        SDL_SetRenderDrawColor(renderer, 125, 125, 125, opacity);
                    }

                    SDL_RenderDrawLine(renderer, layerX, neuronY, layerX + layerSpacing, nextNeuronY);
                }
            }

            // Dessinez le neurone.
            int neuronOpacity = (int)(255.0f * fabs(layer->weights[j]));
            SDL_SetRenderDrawColor(renderer, 0, 200, 161, neuronOpacity);
            if (!cell->isAlive)
            {
                SDL_SetRenderDrawColor(renderer, 125, 125, 125, neuronOpacity);
            }
            SDL_RenderDrawCircle(renderer, layerX, neuronY, 10);
        }
    }

    // Dessinez les inputs (couche d'entrée).
    for (int i = 0; i < nn->layers[0]->neuronCount; i++)
    {
        int inputX = x;
        int inputY = y + (i + 1) * neuronSpacing;
        Uint8 opacity = (Uint8)(cell->inputs[i] * 255);

        // Draw the input neuron
        if (opacity == 0)
        {
            SDL_SetRenderDrawColor(renderer, 255, 180, 53, 125);
            if (!cell->isAlive)
            {
                SDL_SetRenderDrawColor(renderer, 125, 125, 125, 125);
            }
            SDL_RenderDrawCircleOutline(renderer, inputX, inputY, 10);
        }

        SDL_SetRenderDrawColor(renderer, 255, 180, 53, opacity);
        if (!cell->isAlive)
        {
            SDL_SetRenderDrawColor(renderer, 125, 125, 125, opacity);
        }
        SDL_RenderDrawCircle(renderer, inputX, inputY, 10);

        // Dessinez la liaison avec la couche suivante.
        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 50);
        SDL_RenderDrawLine(renderer, inputX, inputY, inputX + layerSpacing, inputY);
    }

    // Dessinez les outputs (couche de sortie).
    NeuralLayer *outputLayer = nn->layers[layerCount - 1];
    for (int i = 0; i < outputLayer->nextLayerNeuronCount; i++)
    {
        int outputX = x + (layerCount + 1) * layerSpacing;
        int outputY = y + (i + 1) * neuronSpacing;
        Uint8 opacity = cell->outputs[i] > 0.5 ? 255 : 125;
        SDL_SetRenderDrawColor(renderer, 125, 125, 255, opacity);
        if (!cell->isAlive)
        {
            SDL_SetRenderDrawColor(renderer, 125, 125, 125, opacity);
        }
        SDL_RenderDrawCircle(renderer, outputX, outputY, 10);

        // Dessinez les liaisons avec la couche précédente.
        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 50);
        for (int j = 0; j < nn->layers[layerCount - 1]->neuronCount; j++)
        {
            int prevNeuronY = y + (j + 1) * neuronSpacing;
            int neuronWeightIndex = i * nn->layers[layerCount - 1]->neuronCount + j;
            float weight = nn->layers[layerCount - 1]->weights[neuronWeightIndex];
            int weightOpacity = (int)(255.0f * fabs(weight) / 10.0f);
            if (weight < 0)
            {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, weightOpacity);
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, weightOpacity);
            }
            if (!cell->isAlive)
            {
                SDL_SetRenderDrawColor(renderer, 125, 125, 125, weightOpacity);
            }
            SDL_RenderDrawLine(renderer, outputX, outputY, outputX - layerSpacing, prevNeuronY);
        }
    }
}
