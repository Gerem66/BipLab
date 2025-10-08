#ifndef GAME_H
#define GAME_H

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_framerate.h>
#include <SDL2/SDL2_gfxPrimitives.h>

typedef struct Map Map;

#include "cell.h"
#include "food.h"
#include "wall.h"
#include "popup.h"
#include "checkpoint.h"
#include "neuralNetwork.h"

// Maximum number in memory
#define CELL_COUNT 1000
#define FOOD_COUNT 50
#define WALL_COUNT 6

// Starting number
#define GAME_START_CELL_COUNT 50
#define GAME_START_FOOD_COUNT 10
#define GAME_START_WALL_COUNT 0

// Game settings
#define CELL_MAX_HEALTH 200
#define CELL_START_HEALTH 100
#define CELL_BIRTH_HEALTH_SACRIFICE 100  // Health points sacrificed for reproduction
#define CELL_BIRTH_SCORE_BONUS 2000      // Score bonus when reproducing
#define CELL_BIRTH_MIN_HEALTH 150        // Minimum health required for reproduction
#define CELL_BIRTH_FAILED_PENALTY 5      // Health penalty for failed reproduction attempt
#define FOOD_MAX_LIMIT 20

// Neural network settings
#define NEURAL_NETWORK_INIT_TOPOLOGY_SIZE 8
#define NEURAL_NETWORK_INIT_TOPOLOGY { 44, 30, 20, 10, 10, 10, 5, 3 }
#define NEURAL_NETWORK_RESET_MUTATION_RATE 0.1f
#define NEURAL_NETWORK_RESET_MUTATION_PROB 0.2f
#define NEURAL_NETWORK_CHILD_MUTATION_RATE 0.05f
#define NEURAL_NETWORK_CHILD_MUTATION_PROB 0.1f
#define NEURAL_NETWORK_TOPOLOGY_MUTATION_PROBABILITY 0.02f
#define NEURAL_NETWORK_TOPOLOGY_LAYER_SIZE_MIN 4
#define NEURAL_NETWORK_TOPOLOGY_LAYER_SIZE_MAX 20
#define NEURAL_NETWORK_TOPOLOGY_NEURON_SIZE_MIN 4
#define NEURAL_NETWORK_TOPOLOGY_NEURON_SIZE_MAX 20

// Settings
#define GAME_FPS_LIMIT      60
#define CELL_USE_SPRITE     true
#define CELL_AS_PLAYER      false
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

    // Checkpoint tracking
    int lastCheckpointGeneration;
    int checkpointCounter;
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
