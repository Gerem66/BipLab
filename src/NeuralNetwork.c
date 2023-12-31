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
    nn->topology[topologySize - 1] = topology[topologySize - 1];

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
                layer->weights[j] = MIN(1, MAX(-1, layer->weights[j] + randomWeight(-0.1f, 0.1f)));
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

void saveNeuralNetwork(NeuralNetwork *nn, char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erreur en ouvrant le fichier");
        return;
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

// By ChatGPT
void NeuralNetwork_Render(Cell *cell, SDL_Renderer *renderer, int index, int x, int y, int w, int h)
{
    // Assurez-vous que cell et cell->nn ne sont pas NULL.
    if (cell == NULL || cell->nn == NULL) {
        return;
    }

    // Show index of cell
    char indexText[50];
    sprintf(indexText, "Best cell: %d, with score: %d", index, cell->score);
    SDL_Color color = { 255, 255, 255, 255 };
    stringRGBA(renderer, x, y, indexText, color.r, color.g, color.b, color.a);

    NeuralNetwork *nn = cell->nn;
    int layerCount = nn->topologySize - 1;

    // Calculez l'espacement entre les layers et les neurones.
    int layerSpacing = w / (layerCount + 2);
    int maxNeurons = 0;
    for (int i = 0; i < nn->topologySize - 1; i++) {
        if (nn->topology[i] > maxNeurons) {
            maxNeurons = nn->topology[i];
        }
    }
    int neuronSpacing = h / (maxNeurons + 1);

    // Dessinez les layers et les neurones.
    for (int i = 0; i < layerCount; i++) {
        int layerX = x + (i + 1) * layerSpacing;
        for (int j = 0; j < nn->topology[i]; j++) {
            int neuronY = y + (j + 1) * neuronSpacing;
            int nextLayerNeuronCount = (i < layerCount - 1) ? nn->topology[i + 1] : 0;

            // Dessinez les liaisons.
            if (i < layerCount - 1) {
                for (int k = 0; k < nextLayerNeuronCount; k++) {
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
                        if (!cell->isAlive)
                            SDL_SetRenderDrawColor(renderer, 125, 125, 125, opacity);
                    }
                    //SDL_SetRenderDrawColor(renderer, 128, 128, 128, 50);
                    SDL_RenderDrawLine(renderer, layerX, neuronY, layerX + layerSpacing, nextNeuronY);
                }
            }

            // Dessinez le neurone.
            int opacity = (int)(255 * fabs(nn->layers[i]->weights[j]));
            SDL_SetRenderDrawColor(renderer, 0, 200, 161, opacity);
            if (!cell->isAlive)
                SDL_SetRenderDrawColor(renderer, 125, 125, 125, opacity);
            SDL_RenderDrawCircle(renderer, layerX, neuronY, 10);
        }
    }

    // Dessinez les inputs.
    // Ajustez selon la façon dont vous voulez afficher les inputs.
    for (int i = 0; i < nn->topology[0]; i++) {
        int inputX = x;
        int inputY = y + (i + 1) * neuronSpacing;
        Uint8 opacity = cell->inputs[i] * 255;

        // Draw outline
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

        // Dessinez les liaisons.
        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 50);
        SDL_RenderDrawLine(renderer, inputX, inputY, inputX + layerSpacing, inputY);
    }

    // Dessinez les outputs.
    // Ajustez selon la façon dont vous voulez afficher les outputs.
    for (int i = 0; i < nn->layers[layerCount - 1]->nextLayerNeuronCount; i++) {
        int outputX = x + (layerCount + 1) * layerSpacing;
        int outputY = y + (i + 1) * neuronSpacing;
        Uint8 opacity = cell->outputs[i] > 0.5 ? 255 : 125;
        SDL_SetRenderDrawColor(renderer, 125, 125, 255, opacity);
        if (!cell->isAlive)
            SDL_SetRenderDrawColor(renderer, 125, 125, 125, opacity);
        SDL_RenderDrawCircle(renderer, outputX, outputY, 10);

        // Dessinez les liaisons.
        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 50);
        for (int j = 0; j < nn->topology[layerCount - 1]; j++) {
            int nextNeuronY = y + (j + 1) * neuronSpacing;
            int neuronWeightIndex = i * nn->topology[layerCount - 1] + j;
            float weight = nn->layers[layerCount - 1]->weights[neuronWeightIndex];
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
                if (!cell->isAlive)
                    SDL_SetRenderDrawColor(renderer, 125, 125, 125, opacity);
            }
            SDL_RenderDrawLine(renderer, outputX, outputY, outputX - layerSpacing, nextNeuronY);
        }
    }
}
