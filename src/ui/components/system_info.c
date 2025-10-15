/**
 * @file system_info.c
 * @brief Implementation of the system information UI component
 */

#include "../../../include/ui/components/system_info.h"
#include "../../../include/ui/components/progressbar.h"
#include "../../../include/ui/components/realtime_graph.h"
#include "../../../include/ui/ui_utils.h"
#include "../../../include/system/hardware_monitor.h"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// Static graph instances for smooth animations
static RealtimeGraph g_cpuGraph;
static RealtimeGraph g_ramGraph;
static RealtimeGraph g_gpuGraph;
static bool g_graphsInitialized = false;

void SystemInfo_RenderPanel(SDL_Renderer *renderer, Map *map, int x, int y)
{
    // Initialize graphs if needed
    if (!g_graphsInitialized) {
        SDL_Color cpuColor = CPU_GRAPH_COLOR;
        SDL_Color ramColor = RAM_GRAPH_COLOR;
        SDL_Color gpuColor = GPU_GRAPH_COLOR;

        RealtimeGraph_Init(&g_cpuGraph, "CPU", cpuColor, 0.0, 100.0);
        RealtimeGraph_Init(&g_ramGraph, "RAM", ramColor, 0.0, 100.0);
        RealtimeGraph_Init(&g_gpuGraph, "GPU", gpuColor, 0.0, 100.0);
        g_graphsInitialized = true;
    }

    // Update hardware monitoring data and graphs
    bool dataUpdated = HwMonitor_Update(map->frames);
    if (dataUpdated) {
        // Add new samples to graphs when hardware data is updated
        double cpuUsage = HwMonitor_GetCurrentCPUUsage();
        double ramUsage = HwMonitor_GetCurrentRAMUsage();
        double gpuUsage = HwMonitor_GetCurrentGPUUsage();

        RealtimeGraph_AddSample(&g_cpuGraph, cpuUsage);
        RealtimeGraph_AddSample(&g_ramGraph, ramUsage);
        RealtimeGraph_AddSample(&g_gpuGraph, gpuUsage);
    }

    int currentY = y - 20;

    // Panel title
    SDL_Color titleColor = {200, 200, 200, 255};
    stringRGBA(renderer, x, currentY, "SYSTEM INFO",
              titleColor.r, titleColor.g, titleColor.b, titleColor.a);
    currentY += 20;

    // Render hardware specifications
    currentY = SystemInfo_RenderSpecs(renderer, x, currentY);
    currentY += 20;

    // Render usage graphs with new smooth graph component
    SystemInfo_RenderUsageGraphs(renderer, x, currentY);
}

int SystemInfo_RenderSpecs(SDL_Renderer *renderer, int x, int y)
{
    const CPUInfo *cpu = HwMonitor_GetCPUInfo();
    const GPUInfo *gpu = HwMonitor_GetGPUInfo();
    const RAMInfo *ram = HwMonitor_GetRAMInfo();

    SDL_Color labelColor = {180, 180, 180, 255};
    SDL_Color valueColor = {255, 255, 255, 255};

    int currentY = y;
    char text[256];

    // CPU Information
    if (cpu) {
        // CPU Model (truncated if too long)
        char cpuModel[64];
        strncpy(cpuModel, cpu->model, sizeof(cpuModel) - 1);
        cpuModel[sizeof(cpuModel) - 1] = '\0';

        sprintf(text, "CPU: %s", cpuModel);
        stringRGBA(renderer, x, currentY, text, valueColor.r, valueColor.g, valueColor.b, valueColor.a);
        currentY += SYSTEM_INFO_LINE_HEIGHT;

        // CPU cores and frequency
        char freqStr[16];
        HwMonitor_FormatFrequency(cpu->maxFrequency, freqStr);
        sprintf(text, "     %d cores, %d threads @ %s",
                cpu->coreCount, cpu->threadCount, freqStr);
        stringRGBA(renderer, x, currentY, text, labelColor.r, labelColor.g, labelColor.b, labelColor.a);
        currentY += SYSTEM_INFO_LINE_HEIGHT;
    }

    // RAM Information
    if (ram) {
        char ramStr[32];
        HwMonitor_FormatMemorySize(ram->totalRAM, ramStr);
        sprintf(text, "RAM: %s total", ramStr);
        stringRGBA(renderer, x, currentY, text, valueColor.r, valueColor.g, valueColor.b, valueColor.a);
        currentY += SYSTEM_INFO_LINE_HEIGHT;
    }

    // GPU Information
    if (gpu && gpu->isAvailable) {
        // GPU Model (truncated if too long)
        char gpuModel[64];
        strncpy(gpuModel, gpu->model, sizeof(gpuModel) - 1);
        gpuModel[sizeof(gpuModel) - 1] = '\0';

        sprintf(text, "GPU: %s", gpuModel);
        stringRGBA(renderer, x, currentY, text, valueColor.r, valueColor.g, valueColor.b, valueColor.a);
        currentY += SYSTEM_INFO_LINE_HEIGHT;

        // GPU memory if available
        if (gpu->totalMemory > 0) {
            char vramStr[32];
            HwMonitor_FormatMemorySize(gpu->totalMemory, vramStr);
            sprintf(text, "     %s VRAM", vramStr);
            stringRGBA(renderer, x, currentY, text, labelColor.r, labelColor.g, labelColor.b, labelColor.a);
            currentY += SYSTEM_INFO_LINE_HEIGHT;
        }
    } else {
        // Check if we're in a toolbox environment for better messaging
        bool inToolbox = getenv("TOOLBOX_PATH") != NULL || getenv("container") != NULL;
        if (inToolbox) {
            sprintf(text, "GPU: Host GPU (limited access)");
        } else {
            sprintf(text, "GPU: Not detected");
        }
        stringRGBA(renderer, x, currentY, text, labelColor.r, labelColor.g, labelColor.b, labelColor.a);
        currentY += SYSTEM_INFO_LINE_HEIGHT;
    }

    return currentY;
}

// Function removed - progress bars were redundant with graphs

int SystemInfo_RenderUsageGraphs(SDL_Renderer *renderer, int x, int y)
{
    SDL_Color labelColor = {180, 180, 180, 255};
    int currentY = y;

    if (!g_graphsInitialized) {
        stringRGBA(renderer, x, currentY, "Initializing graphs...",
                  labelColor.r, labelColor.g, labelColor.b, labelColor.a);
        return currentY + SYSTEM_INFO_LINE_HEIGHT;
    }

    int graphWidth = SYSTEM_INFO_PANEL_WIDTH - 20;
    int graphSpacing = SYSTEM_INFO_GRAPH_HEIGHT + 25;

    // CPU Usage Graph
    RealtimeGraph_Render(renderer, &g_cpuGraph, x, currentY, graphWidth, SYSTEM_INFO_GRAPH_HEIGHT);
    currentY += graphSpacing;

    // RAM Usage Graph
    RealtimeGraph_Render(renderer, &g_ramGraph, x, currentY, graphWidth, SYSTEM_INFO_GRAPH_HEIGHT);
    currentY += graphSpacing;

    // GPU Usage Graph (if available)
    const GPUInfo *gpu = HwMonitor_GetGPUInfo();
    if (gpu && gpu->isAvailable) {
        RealtimeGraph_Render(renderer, &g_gpuGraph, x, currentY, graphWidth, SYSTEM_INFO_GRAPH_HEIGHT);
        currentY += graphSpacing;
    }

    return currentY;
}

// Helper functions

SDL_Color SystemInfo_GetUsageColor(double usage)
{
    if (usage < 50.0) {
        return (SDL_Color){100, 255, 100, 255}; // Green - low usage
    } else if (usage < 80.0) {
        return (SDL_Color){255, 200, 100, 255}; // Yellow - medium usage
    } else {
        return (SDL_Color){255, 100, 100, 255}; // Red - high usage
    }
}

char* SystemInfo_FormatPercentage(double value, char* buffer)
{
    if (value < 10.0) {
        sprintf(buffer, "%.1f%%", value);
    } else {
        sprintf(buffer, "%.0f%%", value);
    }
    return buffer;
}
