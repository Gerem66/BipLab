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
#include "graph.h"

// Screen and game dimensions
#define SCREEN_WIDTH    1200
#define SCREEN_HEIGHT   800
#define GAME_WIDTH      1200
#define GAME_HEIGHT     800

// Maximum number in memory
#define CELL_COUNT 1000
#define FOOD_COUNT 50
#define WALL_COUNT 6

// Starting number
#define GAME_START_CELL_COUNT 50
#define GAME_START_FOOD_COUNT 15
#define GAME_START_WALL_COUNT 0

// Game settings
#define FRAMES_PER_HEALTH_POINT 40
#define FOOD_ITEM_CAPACITY 20

// Cell settings
#define CELL_MAX_HEALTH 100
#define CELL_START_HEALTH 40
#define CELL_BIRTH_HEALTH_SACRIFICE 50   // Health points sacrificed for reproduction
#define CELL_BIRTH_SCORE_BONUS 0.1       // Score bonus when reproducing (in pourcentage of current score)
#define CELL_BIRTH_MIN_HEALTH 75         // Minimum health required for reproduction
#define CELL_BIRTH_FAILED_PENALTY 2      // Health penalty for failed reproduction attempt

// Neural network settings
#define NEURAL_NETWORK_TOPOLOGY_SIZE 12
#define NEURAL_NETWORK_TOPOLOGY { 44, 128, 128, 128, 96, 64, 64, 64, 64, 32, 16, 3 }
#define NEURAL_NETWORK_RESET_MUTATION_RATE 0.1f
#define NEURAL_NETWORK_RESET_MUTATION_PROB 0.2f
#define NEURAL_NETWORK_CHILD_MUTATION_RATE 0.05f
#define NEURAL_NETWORK_CHILD_MUTATION_PROB 0.1f
#define NEURAL_NETWORK_TOPOLOGY_MUTATION_PROBABILITY 0.02f
#define NEURAL_NETWORK_TOPOLOGY_LAYER_SIZE_MIN 4
#define NEURAL_NETWORK_TOPOLOGY_LAYER_SIZE_MAX 20
#define NEURAL_NETWORK_TOPOLOGY_NEURON_SIZE_MIN 4
#define NEURAL_NETWORK_TOPOLOGY_NEURON_SIZE_MAX 20

// Evolution settings
#define EVOLUTION_PARENT_SELECTION_RATIO 0.1f

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
