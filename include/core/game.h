#ifndef GAME_H
#define GAME_H

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_framerate.h>
#include <SDL2/SDL2_gfxPrimitives.h>

typedef struct Map Map;

#include "config.h"
#include "../entities/cell.h"
#include "../entities/food.h"
#include "../entities/wall.h"
#include "../ui/popup.h"
#include "../system/checkpoint.h"
#include "../ai/neuralNetwork.h"
#include "../ui/graph/graphEvolution.h"
#include "../ai/evolution.h"
#include "../ui/graph/graphEvolutionWindow.h"
#include "../ui/interfaces/gameInterface.h"
#include "../ui/interfaces/trainingInterface.h"

struct Map
{
    int width;
    int height;
    SDL_Point viewOffset;
    float zoomFactor;
    bool isDragging;
    SDL_Point dragStartMouse;
    SDL_Point dragStartView;

    time_t startTime;
    time_t pausedTime;

    Cell *cells[MEM_CELL_COUNT];
    Food *foods[MEM_FOOD_COUNT];
    Wall *walls[MEM_WALL_COUNT];
    Cell *bestCellEver;
    int cellCount;
    int generation;
    int maxGeneration;
    int frames;
    int maxScore;
    bool isRunning;
    bool verticalSync;
    bool renderText;
    bool renderRays;
    bool renderNeuralNetwork;
    bool renderScoreGraph;
    bool renderEnabled;

    bool useMultithreading;  // Runtime flag to enable/disable OpenMP multithreading
    bool useGpuAcceleration; // Runtime flag to enable/disable GPU acceleration for training

    // Screen mode
    int mode;

    bool quit;
    int currentBestCellIndex;
    SDL_Renderer *renderer;
    SDL_Window *window;  // Store window for resizing

    // Graph window
    bool graphWindowOpen;
    SDL_Window *graphWindow;
    SDL_Renderer *graphRenderer;

    // Checkpoint tracking
    int lastCheckpointGeneration;
    int checkpointCounter;

    // Performance tracking
    int previousGenFrames;
    int currentFPS;
    int currentUPS;
    float currentGPS;

    // Graph system
    GraphData graphData;

    // Evolution system
    EvolutionMetrics evolutionMetrics;
    DynamicMutationParams mutationParams;
};


bool Game_start(SDL_Window *window, SDL_Renderer *renderer, int w, int h);
void Game_events(Map *map, SDL_Event *event);
void Game_update(Map *map);
void Game_reset(Map *map, bool fullReset);
void Game_ResizeWindow(Map *map, int width, int height);

bool Game_exists(char *filename);
bool Game_save(Map *map, char *filename);
NeuralNetwork* Game_load(Map *map, char *filename);

#endif // GAME_H
