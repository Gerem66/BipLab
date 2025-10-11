#ifndef EVOLUTION_H
#define EVOLUTION_H

#include "config.h"

// Forward declaration
typedef struct Map Map;

// Evolution metrics structure
typedef struct EvolutionMetrics {
    float avgScoreImprovement;          // Average score improvement over last N generations
    float diversityIndex;               // Population diversity metric
    int generationsSinceImprovement;    // Generations since last significant improvement
    float convergenceRate;              // Rate at which population is converging
} EvolutionMetrics;

// Dynamic mutation parameters
typedef struct DynamicMutationParams {
    float resetMutationRate;        // Dynamic version of NEURAL_NETWORK_RESET_MUTATION_RATE
    float resetMutationProb;        // Dynamic version of NEURAL_NETWORK_RESET_MUTATION_PROB
    float childMutationRate;        // Dynamic version of NEURAL_NETWORK_CHILD_MUTATION_RATE
    float childMutationProb;        // Dynamic version of NEURAL_NETWORK_CHILD_MUTATION_PROB
} DynamicMutationParams;

// Function declarations
void Evolution_CalculateMetrics(Map *map, EvolutionMetrics *metrics);
void Evolution_AdaptMutationParams(EvolutionMetrics *metrics, DynamicMutationParams *params);
void Evolution_InitMutationParams(DynamicMutationParams *params);
float Evolution_CalculateDiversity(Map *map);
float Evolution_CalculateConvergenceRate(Map *map);

#endif // EVOLUTION_H
