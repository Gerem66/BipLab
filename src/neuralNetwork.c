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

        // Bias mutation
        for (int j = 0; j < layer->nextLayerNeuronCount; j++)
        {
            if (rand() / (double)RAND_MAX < mutationProbability)
            {
                layer->biases[j] += drand(-mutationRate, mutationRate);
            }
        }
    }
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

// By AI
void NeuralNetwork_Render(Cell *cell, SDL_Renderer *renderer, int index, int x, int y, int w, int h)
{
    if (cell == NULL || cell->nn == NULL)
    {
        return;
    }

    // Show index of cell and legend
    char indexText[50];
    sprintf(indexText, "Best cell: %d, with score: %d", index, cell->score);
    SDL_Color color = {255, 255, 255, 255};
    stringRGBA(renderer, x, y - 30, indexText, color.r, color.g, color.b, color.a);

    // Add legend for bias representation
    stringRGBA(renderer, x, y - 15, "Bias: Green tint(+) Red tint(-)", 200, 200, 200, 255);

    NeuralNetwork *nn = cell->nn;

    // Connection opacity control - Adjust this value to control density-based opacity reduction
    // 0.0f = no reduction (all connections fully visible)
    // 1.0f = maximum reduction (dense networks become very faint)
    float opacityReductionFactor = 0.8f;
    int layerCount = nn->topologySize - 1;

    // Calculate spacing between layers and neurons
    int layerSpacing = w / (layerCount + 2);
    int maxNeurons = 0;

    // Check input layer size (first layer's neuronCount)
    if (nn->layers[0]->neuronCount > maxNeurons)
    {
        maxNeurons = nn->layers[0]->neuronCount;
    }

    // Check all hidden layers
    for (int i = 0; i < layerCount; i++)
    {
        if (nn->layers[i]->nextLayerNeuronCount > maxNeurons)
        {
            maxNeurons = nn->layers[i]->nextLayerNeuronCount;
        }
    }

    int neuronSpacing = h / (maxNeurons + 1);

    // PHASE 1: Draw all connections first (background layer)

    // Draw connections from inputs to first hidden layer
    int inputCount = nn->topology[0];
    int firstHiddenCount = nn->layers[0]->neuronCount;

    // Calculate centered positions for inputs
    int inputStartY = y + (maxNeurons - inputCount + 1) * neuronSpacing / 2;
    int firstHiddenStartY = y + (maxNeurons - firstHiddenCount + 1) * neuronSpacing / 2;

    // Unified opacity values - less connections = more visible
    float connectionCount = (float)(inputCount * firstHiddenCount);
    float visibilityBoost = 1.0f + (200.0f / connectionCount); // Boost for fewer connections
    float baseOpacity = 50.0f * (1.0f - opacityReductionFactor) * visibilityBoost;
    float intensityRange = 100.0f * (1.0f - opacityReductionFactor) * visibilityBoost;

    for (int j = 0; j < inputCount; j++)
    {
        int inputX = x;
        int inputY = inputStartY + (j + 1) * neuronSpacing;

        for (int k = 0; k < firstHiddenCount; k++)
        {
            int firstLayerX = x + layerSpacing;
            int firstLayerY = firstHiddenStartY + (k + 1) * neuronSpacing;
            int weightIndex = j * firstHiddenCount + k;
            float weight = nn->layers[0]->weights[weightIndex];

            float sourceActivation = cell->inputs[j];
            float destActivation = nn->layers[0]->outputs[k];
            float connectionIntensity = fabs(sourceActivation * destActivation * weight);

            int opacity = (int)(baseOpacity + intensityRange * fmin(connectionIntensity, 1.0f));

            if (weight < 0)
                SDL_SetRenderDrawColor(renderer, 220, 80, 80, opacity);
            else
                SDL_SetRenderDrawColor(renderer, 80, 220, 120, opacity);

            if (!cell->isAlive)
                SDL_SetRenderDrawColor(renderer, 100, 100, 100, opacity / 2);

            SDL_RenderDrawLine(renderer, inputX, inputY, firstLayerX, firstLayerY);
        }
    }

    // Draw connections between hidden layers and to output
    for (int i = 0; i < layerCount; i++)
    {
        NeuralLayer *layer = nn->layers[i];
        if (layer->nextLayerNeuronCount == 0) continue;

        int currentSize = (i == 0) ? firstHiddenCount : layer->neuronCount;
        int nextSize = layer->nextLayerNeuronCount;

        // Calculate centered positions
        int currentStartY = y + (maxNeurons - currentSize + 1) * neuronSpacing / 2;
        int nextStartY = y + (maxNeurons - nextSize + 1) * neuronSpacing / 2;

        // Opacity values - less connections = more visible
        float layerConnectionCount = (float)(currentSize * nextSize);
        float layerVisibilityBoost = 1.0f + (200.0f / layerConnectionCount); // Boost for fewer connections
        float layerBaseOpacity = 50.0f * (1.0f - opacityReductionFactor) * layerVisibilityBoost;
        float layerIntensityRange = 100.0f * (1.0f - opacityReductionFactor) * layerVisibilityBoost;
        bool isToOutput = (i == layerCount - 1);

        for (int j = 0; j < currentSize; j++)
        {
            int currentX = x + (i + 1) * layerSpacing;
            int currentY = currentStartY + (j + 1) * neuronSpacing;

            for (int k = 0; k < nextSize; k++)
            {
                int nextX = x + (i + 2) * layerSpacing;
                int nextY = nextStartY + (k + 1) * neuronSpacing;
                int weightIndex = j * nextSize + k;
                float weight = layer->weights[weightIndex];

                float sourceActivation = (i == 0) ? nn->layers[0]->outputs[j] : layer->outputs[j];
                float destActivation = isToOutput ? cell->outputs[k] : nn->layers[i + 1]->outputs[k];
                float connectionIntensity = fabs(sourceActivation * destActivation * weight);

                int opacity = (int)(layerBaseOpacity + layerIntensityRange * fmin(connectionIntensity, 1.0f));

                // Pure control by opacityReductionFactor - no special treatment

                if (weight < 0)
                    SDL_SetRenderDrawColor(renderer, 220, 80, 80, opacity);
                else
                    SDL_SetRenderDrawColor(renderer, 80, 220, 120, opacity);

                if (!cell->isAlive)
                    SDL_SetRenderDrawColor(renderer, 100, 100, 100, opacity / 2);

                SDL_RenderDrawLine(renderer, currentX, currentY, nextX, nextY);
            }
        }
    }

    // PHASE 2: Draw all neurons (foreground layer)

    // First, draw input neurons at position 0
    for (int j = 0; j < inputCount; j++)
    {
        int inputX = x;
        int inputY = inputStartY + (j + 1) * neuronSpacing;
        Uint8 opacity = (Uint8)(cell->inputs[j] * 255);

        if (opacity == 0)
        {
            SDL_SetRenderDrawColor(renderer, 255, 180, 53, 125);
            if (!cell->isAlive)
                SDL_SetRenderDrawColor(renderer, 125, 125, 125, 125);
            SDL_RenderDrawCircleOutline(renderer, inputX, inputY, 10);
        }

        SDL_SetRenderDrawColor(renderer, 255, 180, 53, opacity);
        if (!cell->isAlive)
            SDL_SetRenderDrawColor(renderer, 125, 125, 125, opacity);
        SDL_RenderDrawCircle(renderer, inputX, inputY, 10);
    }

    // Draw all hidden layers
    for (int i = 0; i < layerCount; i++)
    {
        int layerX = x + (i + 1) * layerSpacing;
        NeuralLayer *layer = nn->layers[i];

        // Each layer draws its own neurons (the ones that have outputs)
        int currentSize = (i == 0) ? firstHiddenCount : layer->neuronCount;
        int currentStartY = y + (maxNeurons - currentSize + 1) * neuronSpacing / 2;

        for (int j = 0; j < currentSize; j++)
        {
            int neuronY = currentStartY + (j + 1) * neuronSpacing;

            // Get the correct activation value
            float activation = (i == 0) ? nn->layers[0]->outputs[j] : layer->outputs[j];
            int neuronOpacity = (int)(255.0f * (fabs(activation) + 1.0f) / 2.0f);

            // Color scheme: hidden layers vs output layer
            int red, green, blue;

            if (layer->nextLayerNeuronCount == 0)
            {
                // This is the output layer (last layer has nextLayerNeuronCount = 0)
                red = 125; green = 125; blue = 255;

                // Modulate based on output bias
                float bias = layer->biases[j];
                if (bias > 0.1f)
                {
                    green = (int)(125 + 80 * fmin(bias, 1.0f)); // More green for positive bias
                }
                else if (bias < -0.1f)
                {
                    red = (int)(125 + 80 * fmin(fabs(bias), 1.0f)); // More red for negative bias
                }
            }
            else
            {
                // Hidden layer neurons
                red = 0; green = 200; blue = 161;

                // Modulate based on bias (for i=0, use layer biases; for others, use previous layer's next layer biases)
                float bias = (i == 0) ? layer->biases[j] : layer->biases[j];
                if (bias > 0.1f)
                {
                    green = (int)(200 + 55 * fmin(bias, 1.0f)); // More green for positive bias
                }
                else if (bias < -0.1f)
                {
                    red = (int)(55 * fmin(fabs(bias), 1.0f)); // Some red for negative bias
                }
            }

            SDL_SetRenderDrawColor(renderer, red, green, blue, neuronOpacity);
            if (!cell->isAlive)
            {
                SDL_SetRenderDrawColor(renderer, 125, 125, 125, neuronOpacity);
            }
            SDL_RenderDrawCircle(renderer, layerX, neuronY, 10);
        }
    }

    // Draw output neurons (final layer)
    NeuralLayer *outputLayer = nn->layers[layerCount - 1];
    int outputX = x + (layerCount + 1) * layerSpacing;
    int outputStartY = y + (maxNeurons - outputLayer->nextLayerNeuronCount + 1) * neuronSpacing / 2;

    for (int i = 0; i < outputLayer->nextLayerNeuronCount; i++)
    {
        int outputY = outputStartY + (i + 1) * neuronSpacing;
        Uint8 opacity = cell->outputs[i] > 0.5 ? 255 : 125;

        // Output neurons are blue with bias modulation
        int red = 125, green = 125, blue = 255;
        float outputBias = outputLayer->biases[i];

        if (outputBias > 0.1f)
        {
            green = (int)(125 + 80 * fmin(outputBias, 1.0f)); // More green for positive bias
        }
        else if (outputBias < -0.1f)
        {
            red = (int)(125 + 80 * fmin(fabs(outputBias), 1.0f)); // More red for negative bias
        }

        SDL_SetRenderDrawColor(renderer, red, green, blue, opacity);
        if (!cell->isAlive)
        {
            SDL_SetRenderDrawColor(renderer, 125, 125, 125, opacity);
        }
        SDL_RenderDrawCircle(renderer, outputX, outputY, 10);
    }
}
