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
#include "cell.h"
#include "food.h"
#include "wall.h"
#include "popup.h"
#include "checkpoint.h"
#include "neuralNetwork.h"
#include "graph.h"
#include "evolution.h"

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

    Cell *cells[CELL_COUNT];
    Food *foods[FOOD_COUNT];
    Wall *walls[WALL_COUNT];
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

    bool quit;
    int currentBestCellIndex;
    SDL_Renderer *renderer;

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


bool Game_start(SDL_Renderer *renderer, int w, int h);
void Game_events(Map *map, SDL_Event *event);
void Game_update(Map *map);
void Game_reset(Map *map, bool fullReset);
void Game_render(SDL_Renderer *renderer, Map *map);
void Render_Text(Map *map, SDL_Color color);
void Render_ZoomBar(Map *map, SDL_Color color, int x, int y);

bool Game_exists(char *filename);
bool Game_save(Map *map, char *filename);
NeuralNetwork* Game_load(Map *map, char *filename);

#endif // GAME_H
