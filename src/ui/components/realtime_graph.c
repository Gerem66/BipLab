/**
 * @file realtime_graph.c
 * @brief Implementation of the real-time graph component
 */

#include "../../../include/ui/components/realtime_graph.h"
#include "../../../include/ui/ui_utils.h"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <string.h>
#include <stdio.h>
#include <math.h>


void RealtimeGraph_Init(RealtimeGraph* graph, const char* title, SDL_Color color,
                       double minValue, double maxValue)
{
    memset(graph, 0, sizeof(RealtimeGraph));

    strncpy(graph->title, title, sizeof(graph->title) - 1);
    graph->title[sizeof(graph->title) - 1] = '\0';

    graph->color = color;
    graph->minValue = minValue;
    graph->maxValue = maxValue;

    graph->sampleCount = 0;
    graph->currentIndex = 0;
    graph->currentValue = 0.0;
}

void RealtimeGraph_AddSample(RealtimeGraph* graph, double value)
{
    // Clamp value to valid range
    value = fmax(graph->minValue, fmin(graph->maxValue, value));

    // Store current raw value
    graph->currentValue = value;

    // Add to circular buffer
    graph->samples[graph->currentIndex] = value;
    graph->currentIndex = (graph->currentIndex + 1) % GRAPH_MAX_SAMPLES;

    if (graph->sampleCount < GRAPH_MAX_SAMPLES) {
        graph->sampleCount++;
    }
}

void RealtimeGraph_Render(SDL_Renderer* renderer, RealtimeGraph* graph,
                         int x, int y, int width, int height)
{
    if (!graph || graph->sampleCount == 0) {
        // Draw empty graph with just background and title
        SDL_Color bgColor = GRAPH_BG_COLOR;
        DrawSmoothRoundedRect(renderer, x, y, width, height, 4, bgColor);

        SDL_Color textColor = GRAPH_TEXT_COLOR;
        char titleText[128];
        sprintf(titleText, "%s: No data", graph ? graph->title : "Graph");
        stringRGBA(renderer, x + 5, y - 18, titleText,
                  textColor.r, textColor.g, textColor.b, textColor.a);
        return;
    }

    // Draw background
    SDL_Color bgColor = GRAPH_BG_COLOR;
    DrawSmoothRoundedRect(renderer, x, y, width, height, 4, bgColor);

    // Draw title with current value
    SDL_Color textColor = GRAPH_TEXT_COLOR;
    char titleText[128];
    if (graph->currentValue < 10.0) {
        sprintf(titleText, "%s: %.1f%%", graph->title, graph->currentValue);
    } else {
        sprintf(titleText, "%s: %.0f%%", graph->title, graph->currentValue);
    }
    stringRGBA(renderer, x + 5, y - 18, titleText,
              textColor.r, textColor.g, textColor.b, textColor.a);

    // Draw grid lines (25%, 50%, 75%)
    SDL_Color gridColor = GRAPH_GRID_COLOR;
    SDL_SetRenderDrawColor(renderer, gridColor.r, gridColor.g, gridColor.b, gridColor.a);
    for (int i = 1; i < 4; i++) {
        int gridY = y + height - (height * i / 4);
        SDL_RenderDrawLine(renderer, x + 2, gridY, x + width - 2, gridY);
    }

    if (graph->sampleCount < 2) {
        return; // Need at least 2 points to draw a line
    }

    // Calculate step size for x-axis
    double stepX = (double)(width - 4) / (double)(graph->sampleCount - 1);
    double valueRange = graph->maxValue - graph->minValue;

    // First pass: Draw filled area under the curve
    for (int i = 0; i < graph->sampleCount - 1; i++) {
        int dataIndex1 = (graph->currentIndex - graph->sampleCount + i + GRAPH_MAX_SAMPLES) % GRAPH_MAX_SAMPLES;
        int dataIndex2 = (graph->currentIndex - graph->sampleCount + i + 1 + GRAPH_MAX_SAMPLES) % GRAPH_MAX_SAMPLES;

        double value1 = graph->samples[dataIndex1];
        double value2 = graph->samples[dataIndex2];

        // For the last segment, use the current raw value
        if (i == graph->sampleCount - 2) {
            value2 = graph->currentValue;
        }

        int x1 = x + 2 + (int)(i * stepX);
        int y1 = y + height - 2 - (int)(((value1 - graph->minValue) / valueRange) * (height - 4));
        int x2 = x + 2 + (int)((i + 1) * stepX);
        int y2 = y + height - 2 - (int)(((value2 - graph->minValue) / valueRange) * (height - 4));

        // Draw filled area under curve
        int baseY = y + height - 2;
        Sint16 vx[4] = {(Sint16)x1, (Sint16)x2, (Sint16)x2, (Sint16)x1};
        Sint16 vy[4] = {(Sint16)y1, (Sint16)y2, (Sint16)baseY, (Sint16)baseY};
        filledPolygonRGBA(renderer, vx, vy, 4,
                         graph->color.r, graph->color.g, graph->color.b, 40);
    }

    // Second pass: Draw the curve line
    for (int i = 0; i < graph->sampleCount - 1; i++) {
        int dataIndex1 = (graph->currentIndex - graph->sampleCount + i + GRAPH_MAX_SAMPLES) % GRAPH_MAX_SAMPLES;
        int dataIndex2 = (graph->currentIndex - graph->sampleCount + i + 1 + GRAPH_MAX_SAMPLES) % GRAPH_MAX_SAMPLES;

        double value1 = graph->samples[dataIndex1];
        double value2 = graph->samples[dataIndex2];

        // For the last segment, use the current raw value
        if (i == graph->sampleCount - 2) {
            value2 = graph->currentValue;
        }

        int x1 = x + 2 + (int)(i * stepX);
        int y1 = y + height - 2 - (int)(((value1 - graph->minValue) / valueRange) * (height - 4));
        int x2 = x + 2 + (int)((i + 1) * stepX);
        int y2 = y + height - 2 - (int)(((value2 - graph->minValue) / valueRange) * (height - 4));

        // Draw smooth thick line
        thickLineRGBA(renderer, x1, y1, x2, y2, 2,
                     graph->color.r, graph->color.g, graph->color.b, 255);
    }

    // Draw current value indicator
    double clampedValue = fmax(graph->minValue, fmin(graph->maxValue, graph->currentValue));
    int indicatorX = x + width - 3;
    int indicatorY = y + height - 2 - (int)(((clampedValue - graph->minValue) / valueRange) * (height - 4));

    // Draw elegant vertical line indicator
    SDL_SetRenderDrawColor(renderer, graph->color.r, graph->color.g, graph->color.b, 200);
    SDL_RenderDrawLine(renderer, indicatorX, indicatorY - 2, indicatorX, indicatorY + 2);
    SDL_SetRenderDrawColor(renderer, graph->color.r, graph->color.g, graph->color.b, 120);
    SDL_RenderDrawLine(renderer, indicatorX - 1, indicatorY - 1, indicatorX - 1, indicatorY + 1);
    SDL_RenderDrawLine(renderer, indicatorX + 1, indicatorY - 1, indicatorX + 1, indicatorY + 1);
}

double RealtimeGraph_GetCurrentValue(RealtimeGraph* graph)
{
    return graph ? graph->currentValue : 0.0;
}

void RealtimeGraph_Clear(RealtimeGraph* graph)
{
    if (graph) {
        graph->sampleCount = 0;
        graph->currentIndex = 0;
        graph->currentValue = 0.0;
    }
}
