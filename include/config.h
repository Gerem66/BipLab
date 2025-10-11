#ifndef CONFIG_H
#define CONFIG_H

// =============================================================================
// MARK: SCREEN AND GAME CONFIGURATION
// =============================================================================

#define SCREEN_WIDTH    1200
#define SCREEN_HEIGHT   800
#define GAME_WIDTH      1200
#define GAME_HEIGHT     800

// =============================================================================
// MARK: GAME OBJECTS LIMITS
// =============================================================================

#define CELL_COUNT 1000
#define FOOD_COUNT 50
#define WALL_COUNT 6

#define GAME_START_CELL_COUNT 50
#define GAME_START_FOOD_COUNT 15
#define GAME_START_WALL_COUNT 0

// =============================================================================
// MARK: GAME MECHANICS
// =============================================================================

#define FRAMES_PER_HEALTH_POINT 40
#define FOOD_ITEM_CAPACITY 20

#define CELL_MAX_HEALTH 100
#define CELL_START_HEALTH 40
#define CELL_BIRTH_HEALTH_SACRIFICE 50   // Health points sacrificed for reproduction
#define CELL_BIRTH_SCORE_BONUS 0.1       // Score bonus when reproducing (in percentage of current score)
#define CELL_BIRTH_MIN_HEALTH 75         // Minimum health required for reproduction
#define CELL_BIRTH_FAILED_PENALTY 2      // Health penalty for failed reproduction attempt

// =============================================================================
// MARK: NEURAL NETWORK CONFIGURATION
// =============================================================================

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

// =============================================================================
// MARK: EVOLUTION ALGORITHM
// =============================================================================

#define EVOLUTION_PARENT_SELECTION_RATIO 0.1f

// =============================================================================
// MARK: GRAPHICS AND CONTROLS
// =============================================================================

#define GAME_FPS_LIMIT      60
#define CELL_USE_SPRITE     true
#define CELL_AS_PLAYER      false
#define CONTROLS_ZOOM_SPEED 0.2

#endif // CONFIG_H
