#include "../../../include/ui/graph/neuralNetworkRender.h"
#include "../../../include/ai/neuralNetwork.h"
#include "../../../include/core/utils.h"
#include "../../../include/ui/ui_utils.h"

#include <SDL2/SDL2_gfxPrimitives.h>

void NeuralNetworkRender_Draw(Cell *cell, SDL_Renderer *renderer, int index, int x, int y, int w, int h)
{
    if (cell == NULL || cell->nn == NULL)
    {
        return;
    }

    NeuralNetwork *nn = cell->nn;

    // Show index of cell and legend
    char indexText[50];
    sprintf(indexText, "Best cell: %d, with score: %d", index, cell->score);
    SDL_Color color = {255, 255, 255, 255};
    stringRGBA(renderer, x, y - 30, indexText, color.r, color.g, color.b, color.a);
    stringRGBA(renderer, x, y - 15, "Bias: Green tint(+) Red tint(-)", 200, 200, 200, 255);

    // Network topology and layout calculations
    int layerCount = nn->topologySize - 1;
    int layerSpacing = (w - 20) / (layerCount + 1);

    // Find maximum neurons for centering
    int maxNeurons = 0;
    for (int i = 0; i < nn->topologySize; i++)
    {
        if (nn->topology[i] > maxNeurons)
            maxNeurons = nn->topology[i];
    }

    int neuronSpacing = h / (maxNeurons + 1);
    float opacityReductionFactor = 0.8f;

    // Unified opacity calculation
    float baseOpacity = 50.0f * (1.0f - opacityReductionFactor);
    float intensityRange = 100.0f * (1.0f - opacityReductionFactor);

    // PHASE 1: Draw all connections (background layer)
    for (int layerIdx = 0; layerIdx < layerCount; layerIdx++)
    {
        NeuralLayer *layer = nn->layers[layerIdx];

        // Source layer info (what feeds into this layer)
        int sourceSize = nn->topology[layerIdx];
        int sourceStartY = y + (maxNeurons - sourceSize + 1) * neuronSpacing / 2;
        int sourceX = x + layerIdx * layerSpacing;

        // Destination layer info (what this layer outputs to)
        int destSize = nn->topology[layerIdx + 1];
        int destStartY = y + (maxNeurons - destSize + 1) * neuronSpacing / 2;
        int destX = x + (layerIdx + 1) * layerSpacing;

        // Draw connections
        for (int srcIdx = 0; srcIdx < sourceSize; srcIdx++)
        {
            int srcY = sourceStartY + (srcIdx + 1) * neuronSpacing;

            for (int destIdx = 0; destIdx < destSize; destIdx++)
            {
                int destY = destStartY + (destIdx + 1) * neuronSpacing;
                int weightIdx = srcIdx * destSize + destIdx;
                float weight = layer->weights[weightIdx];

                // Calculate connection intensity
                float srcActivation = (layerIdx == 0) ? cell->inputs[srcIdx] : nn->layers[layerIdx - 1]->outputs[srcIdx];
                float destActivation = layer->outputs[destIdx];
                float intensity = fabs(srcActivation * destActivation * weight);

                int opacity = (int)(baseOpacity + intensityRange * fmin(intensity, 1.0f));

                // Color based on weight sign
                if (weight < 0)
                    SDL_SetRenderDrawColor(renderer, 220, 80, 80, opacity);
                else
                    SDL_SetRenderDrawColor(renderer, 80, 220, 120, opacity);

                if (!cell->isAlive)
                    SDL_SetRenderDrawColor(renderer, 100, 100, 100, opacity / 2);

                SDL_RenderDrawLine(renderer, sourceX, srcY, destX, destY);
            }
        }
    }

    // PHASE 2: Draw all neurons (foreground layer)
    for (int layerIdx = 0; layerIdx <= layerCount; layerIdx++)
    {
        int neuronCount = nn->topology[layerIdx];
        int neuronStartY = y + (maxNeurons - neuronCount + 1) * neuronSpacing / 2;
        int neuronX = x + layerIdx * layerSpacing;

        for (int neuronIdx = 0; neuronIdx < neuronCount; neuronIdx++)
        {
            int neuronY = neuronStartY + (neuronIdx + 1) * neuronSpacing;

            // Get activation and determine colors
            float activation;
            int red, green, blue;
            Uint8 opacity;

            if (layerIdx == 0)
            {
                // Input layer (orange)
                activation = cell->inputs[neuronIdx];
                opacity = (Uint8)(activation * 255);
                red = 255; green = 180; blue = 53;

                // Draw outline for inactive inputs
                if (opacity == 0)
                {
                    SDL_SetRenderDrawColor(renderer, red, green, blue, 125);
                    if (!cell->isAlive)
                        SDL_SetRenderDrawColor(renderer, 125, 125, 125, 125);
                    SDL_RenderDrawCircleOutline(renderer, neuronX, neuronY, 10);
                }
            }
            else if (layerIdx == layerCount)
            {
                // Output layer (blue)
                activation = cell->outputs[neuronIdx];
                opacity = activation > 0.5 ? 255 : 125;
                red = 125; green = 125; blue = 255;

                // Modulate color based on bias
                float bias = nn->layers[layerIdx - 1]->biases[neuronIdx];
                if (bias > 0.1f)
                    green = (int)(125 + 80 * fmin(bias, 1.0f));
                else if (bias < -0.1f)
                    red = (int)(125 + 80 * fmin(fabs(bias), 1.0f));
            }
            else
            {
                // Hidden layer (green)
                activation = nn->layers[layerIdx - 1]->outputs[neuronIdx];
                opacity = (Uint8)(255.0f * (fabs(activation) + 1.0f) / 2.0f);
                red = 0; green = 200; blue = 161;

                // Modulate color based on bias
                float bias = nn->layers[layerIdx - 1]->biases[neuronIdx];
                if (bias > 0.1f)
                    green = (int)(200 + 55 * fmin(bias, 1.0f));
                else if (bias < -0.1f)
                    red = (int)(55 * fmin(fabs(bias), 1.0f));
            }

            SDL_SetRenderDrawColor(renderer, red, green, blue, opacity);
            if (!cell->isAlive)
                SDL_SetRenderDrawColor(renderer, 125, 125, 125, opacity);

            SDL_RenderDrawCircle(renderer, neuronX, neuronY, 10);
        }
    }
}
