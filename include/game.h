#ifndef GAME_H
#define GAME_H

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <SDL.h>
#include <SDL2_framerate.h>
#include <SDL2_gfxPrimitives.h>

typedef struct Map Map;

#include "cell.h"
#include "food.h"
#include "wall.h"
#include "popup.h"
#include "neuralNetwork.h"

// Maximum number in memory
#define CELL_COUNT 1000
#define FOOD_COUNT 100
#define WALL_COUNT 3

// Starting number
#define GAME_START_CELL_COUNT 100
#define GAME_START_FOOD_COUNT 30

// Settings
#define CONTROLS_ZOOM_SPEED 0.2


struct Map
{
    int width;
    int height;
    SDL_Point viewOffset;
    float zoomFactor;

    time_t startTime;
    Cell *cells[CELL_COUNT];
    Food *foods[FOOD_COUNT];
    Wall *walls[WALL_COUNT];
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
    bool renderEnabled;

    bool quit;
    int currentBestCellIndex;
    SDL_Renderer *renderer;
};


bool Game_start(SDL_Renderer *renderer, int w, int h);
void Game_events(Map *map, SDL_Event *event);
void Game_update(Map *map);
void Game_reset(Map *map);
void Game_render(SDL_Renderer *renderer, Map *map);
void Render_Text(Map *map, SDL_Color color);

bool saveNeuralNetwork(NeuralNetwork *nn, char *filename);
NeuralNetwork* loadNeuralNetwork(char *filename);

#endif // GAME_H
