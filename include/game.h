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
#define FOOD_COUNT 50
#define WALL_COUNT 6

// Starting number
#define GAME_START_CELL_COUNT 20
#define GAME_START_FOOD_COUNT 10
#define GAME_START_WALL_COUNT 0

// Neural network settings
#define NEURAL_NETWORK_INIT_TOPOLOGY_SIZE 4
#define NEURAL_NETWORK_INIT_TOPOLOGY { 15, 10, 8, 4 }
#define NEURAL_NETWORK_MUTATION_RATE_RESET 0.1
#define NEURAL_NETWORK_MUTATION_RATE_CHILD 0.05
#define NEURAL_NETWORK_TOPOLOGY_MUTATION_RATE_RESET 0.1
#define NEURAL_NETWORK_TOPOLOGY_MUTATION_RATE_CHILD 0.05
#define NEURAL_NETWORK_TOPOLOGY_LAYER_SIZE_MIN 4
#define NEURAL_NETWORK_TOPOLOGY_LAYER_SIZE_MAX 20
#define NEURAL_NETWORK_TOPOLOGY_NEURON_SIZE_MIN 4
#define NEURAL_NETWORK_TOPOLOGY_NEURON_SIZE_MAX 20

// Settings
#define CELL_USE_SPRITE     false
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
    bool renderEnabled;

    bool quit;
    int currentBestCellIndex;
    SDL_Renderer *renderer;
};


bool Game_start(SDL_Renderer *renderer, int w, int h);
void Game_events(Map *map, SDL_Event *event);
void Game_update(Map *map);
void Game_reset(Map *map, bool fullReset);
void Game_render(SDL_Renderer *renderer, Map *map);
void Render_Text(Map *map, SDL_Color color);

bool Game_exists(char *filename);
bool Game_save(Map *map, char *filename);
NeuralNetwork* Game_load(Map *map, char *filename);

#endif // GAME_H
