#ifndef CONFIG_H
#define CONFIG_H

// =============================================================================
// MARK: SCREEN AND GAME CONFIGURATION
// =============================================================================

#define SCREEN_WIDTH    1200
#define SCREEN_HEIGHT   800
#define GAME_WIDTH      1200
#define GAME_HEIGHT     800
#define GAME_FPS_LIMIT  60

// =============================================================================
// MARK: GAME OBJECTS LIMITS
// =============================================================================

#define MEM_CELL_COUNT 1000
#define MEM_FOOD_COUNT 50
#define MEM_WALL_COUNT 6

#define GAME_START_CELL_COUNT 50
#define GAME_START_FOOD_COUNT 20
#define GAME_START_WALL_COUNT 0

// =============================================================================
// MARK: GAME MECHANICS
// =============================================================================

// Cell
#define CELL_PERCEPTION_RAYS 7
#define CELL_MAX_HEALTH 100
#define CELL_START_HEALTH 40
#define CELL_HEALTH_DECAY_FRAMES 40     // Frames between each health decay
#define CELL_BIRTH_SCORE_BONUS 0.1      // Score bonus when reproducing (in percentage of current score)
#define CELL_BIRTH_MIN_HEALTH 75        // Minimum health required for reproduction
#define CELL_BIRTH_HEALTH_SACRIFICE 50  // Health points sacrificed for reproduction
#define CELL_BIRTH_FAILED_PENALTY 2     // Health penalty for failed reproduction attempt

// Food item
#define FOOD_ITEM_CAPACITY 20

// =============================================================================
// MARK: NEURAL NETWORK CONFIGURATION
// =============================================================================

// Neural network topology:
// Input layer (30 neurons):
//   - Cell health: 1 input [0.0, 1.0]
//   - Can reproduce: 1 input [0 or 1]
//   - Ray sensors: 7 rays × 4 inputs each = 28 inputs
//     Each ray detects closest object and provides: distance [0.0, 1.0],
//     object type one-hot encoded: wall [0 or 1], food [0 or 1], cell [0 or 1]
// Hidden layers: e.g. 128, 128, 128 neurons
// Output layer (3 neurons):
//   - Acceleration: [-1.0, 1.0] (negative = backward, positive = forward)
//   - Rotation: [-1.0, 1.0] (negative = left, positive = right)
//   - Reproduce: [0.0, 1.0] with threshold at 0.5
#define NEURAL_NETWORK_TOPOLOGY { 44, 128, 128, 128, 3 }

// Percentage of top performers selected as parents for next generation
#define EVOLUTION_PARENT_SELECTION_RATIO 0.1f

// Mutation rate bounds and defaults: [min, default, max]
#define MUTATION_RATE_BOUNDS        { 0.01f, 0.1f, 1.0f }
#define MUTATION_PROB_BOUNDS        { 0.01f, 0.2f, 0.8f }
#define CHILD_MUTATION_RATE_BOUNDS  { 0.01f, 0.05f, 0.5f }
#define CHILD_MUTATION_PROB_BOUNDS  { 0.01f, 0.1f, 0.5f }

// Evolution algorithm constants
#define IMPROVEMENT_HISTORY_SIZE    10
#define SIGNIFICANT_IMPROVEMENT_THRESHOLD 0.05f  // 5% improvement required to consider it significant

// =============================================================================
// MARK: GRAPHICS AND CONTROLS
// =============================================================================

#define CELL_USE_SPRITE     true
#define CELL_AS_PLAYER      false
#define CONTROLS_ZOOM_SPEED 0.2
#define CONTROLS_ZOOM_MIN   1.0f
#define CONTROLS_ZOOM_MAX   5.0f

#endif // CONFIG_H
