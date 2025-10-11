#include "evolution.h"
#include "game.h"
#include <math.h>
#include <float.h>

// Improvement history size (in generations) to consider for average improvement
#define IMPROVEMENT_HISTORY_SIZE 10
// 5% improvement required to consider it "significant"
// Ex: score 100 -> 105 = 5% improvement
// Used to detect real progress vs random fluctuations
#define SIGNIFICANT_IMPROVEMENT_THRESHOLD 0.05f
#define MIN_MUTATION_RATE 0.01f
#define MAX_MUTATION_RATE 1.0f   // Increased for extreme stagnation cases
#define MIN_MUTATION_PROB 0.01f
#define MAX_MUTATION_PROB 0.8f   // Increased for extreme stagnation cases

void Evolution_InitMutationParams(DynamicMutationParams *params)
{
    // Initialize with default config values
    params->resetMutationRate = NEURAL_NETWORK_RESET_MUTATION_RATE;
    params->resetMutationProb = NEURAL_NETWORK_RESET_MUTATION_PROB;
    params->childMutationRate = NEURAL_NETWORK_CHILD_MUTATION_RATE;
    params->childMutationProb = NEURAL_NETWORK_CHILD_MUTATION_PROB;
}

void Evolution_CalculateMetrics(Map *map, EvolutionMetrics *metrics)
{
    // Calculate average score improvement over last few generations
    metrics->avgScoreImprovement = 0.0f;
    if (map->graphData.historyCount >= 2) {
        int recentCount = (map->graphData.historyCount < IMPROVEMENT_HISTORY_SIZE) ?
                         map->graphData.historyCount : IMPROVEMENT_HISTORY_SIZE;

        float totalImprovement = 0.0f;
        int validComparisons = 0;

        for (int i = 1; i < recentCount; i++) {
            int currentIdx = map->graphData.historyCount - i - 1;
            int prevIdx = map->graphData.historyCount - i - 2;

            if (currentIdx >= 0 && prevIdx >= 0 && map->graphData.scoreHistory[prevIdx] > 0) {
                float improvement = (float)(map->graphData.scoreHistory[currentIdx] -
                                          map->graphData.scoreHistory[prevIdx]) /
                                   (float)map->graphData.scoreHistory[prevIdx];
                totalImprovement += improvement;
                validComparisons++;
            }
        }

        if (validComparisons > 0) {
            metrics->avgScoreImprovement = totalImprovement / validComparisons;
        }
    }

    // Calculate diversity index
    metrics->diversityIndex = Evolution_CalculateDiversity(map);

    // Calculate generations since last significant improvement (fixed logic)
    metrics->generationsSinceImprovement = 0;
    if (map->graphData.historyCount >= 2) {
        // Look for the most recent significant improvement
        float recentBestScore = (float)map->graphData.scoreHistory[map->graphData.historyCount - 1];

        // Check recent history first (last 5 generations) for any improvement
        bool recentImprovement = false;
        if (map->graphData.historyCount >= 5) {
            for (int i = 1; i <= 5 && i < map->graphData.historyCount; i++) {
                int idx = map->graphData.historyCount - 1 - i;
                if (idx >= 0) {
                    float oldScore = (float)map->graphData.scoreHistory[idx];
                    if (oldScore > 0 && (recentBestScore - oldScore) / oldScore >= SIGNIFICANT_IMPROVEMENT_THRESHOLD) {
                        recentImprovement = true;
                        metrics->generationsSinceImprovement = i - 1; // Found improvement i generations ago
                        break;
                    }
                }
            }
        }

        // If no recent improvement found, look further back
        if (!recentImprovement) {
            float historicalMax = 0.0f;

            // Find historical maximum before recent window
            int lookbackStart = fmax(0, map->graphData.historyCount - 20); // Look back max 20 generations
            for (int i = 0; i < lookbackStart; i++) {
                float score = (float)map->graphData.scoreHistory[i];
                if (score > historicalMax) {
                    historicalMax = score;
                }
            }

            // Check if recent performance beats historical max significantly
            if (historicalMax > 0 && (recentBestScore - historicalMax) / historicalMax >= SIGNIFICANT_IMPROVEMENT_THRESHOLD) {
                metrics->generationsSinceImprovement = 0; // Recent performance is a breakthrough!
            } else {
                // Count generations since we last beat the historical record
                metrics->generationsSinceImprovement = fmin(map->graphData.historyCount - 1, 20);

                // Look for the last time we had a real breakthrough
                for (int i = map->graphData.historyCount - 2; i >= lookbackStart; i--) {
                    float currentScore = (float)map->graphData.scoreHistory[i];
                    if (currentScore > historicalMax * (1.0f + SIGNIFICANT_IMPROVEMENT_THRESHOLD)) {
                        metrics->generationsSinceImprovement = (map->graphData.historyCount - 1) - i;
                        break;
                    }
                }
            }
        }
    }

    // Calculate convergence rate
    metrics->convergenceRate = Evolution_CalculateConvergenceRate(map);
}

float Evolution_CalculateDiversity(Map *map)
{
    // Calculate diversity based on score variance using coefficient of variation
    float totalScore = 0.0f;
    int validCells = 0;
    float minScore = FLT_MAX;
    float maxScore = 0.0f;

    // Calculate mean score and find min/max
    for (int i = 0; i < map->cellCount; i++) {
        if (map->cells[i] != NULL) {
            float score = (float)map->cells[i]->score;
            totalScore += score;
            validCells++;

            if (score < minScore) minScore = score;
            if (score > maxScore) maxScore = score;
        }
    }

    if (validCells <= 1) return 0.0f;

    float meanScore = totalScore / validCells;

    // Calculate variance
    float variance = 0.0f;
    for (int i = 0; i < map->cellCount; i++) {
        if (map->cells[i] != NULL) {
            float diff = (float)map->cells[i]->score - meanScore;
            variance += diff * diff;
        }
    }
    variance /= validCells;

    // Calculate diversity using coefficient of variation (more robust)
    float standardDev = sqrtf(variance);
    float diversityIndex = 0.0f;

    if (meanScore > 0) {
        // Coefficient of variation method
        diversityIndex = standardDev / meanScore;
    } else if (maxScore > minScore) {
        // Fallback: normalized range method
        diversityIndex = (maxScore - minScore) / (maxScore + 1.0f);
    }

    // Clamp between 0 and 1, with reasonable scaling
    diversityIndex = fminf(1.0f, diversityIndex * 2.0f); // Scale up for better sensitivity
    return fmaxf(0.0f, diversityIndex);
}

float Evolution_CalculateConvergenceRate(Map *map)
{
    // Calculate how quickly the population is converging (less diversity over time)
    if (map->graphData.historyCount < 5) return 0.0f;

    // Look at diversity trend over recent generations
    // For simplicity, we'll use the coefficient of variation of recent scores
    int recentGenerations = (map->graphData.historyCount < 10) ?
                           map->graphData.historyCount : 10;

    float recentVariance = 0.0f;
    float recentMean = 0.0f;

    // Calculate mean of recent scores
    for (int i = 0; i < recentGenerations; i++) {
        int idx = map->graphData.historyCount - 1 - i;
        if (idx >= 0) {
            recentMean += (float)map->graphData.scoreHistory[idx];
        }
    }
    recentMean /= recentGenerations;

    // Calculate variance of recent scores
    for (int i = 0; i < recentGenerations; i++) {
        int idx = map->graphData.historyCount - 1 - i;
        if (idx >= 0) {
            float diff = (float)map->graphData.scoreHistory[idx] - recentMean;
            recentVariance += diff * diff;
        }
    }
    recentVariance /= recentGenerations;

    // Calculate coefficient of variation (normalized variance)
    float coefficientOfVariation = sqrtf(recentVariance) / (recentMean + 1.0f);

    // Lower coefficient of variation = higher convergence
    return fmaxf(0.0f, 1.0f - coefficientOfVariation);
}

void Evolution_AdaptMutationParams(EvolutionMetrics *metrics, DynamicMutationParams *params)
{
    // Base adaptation strategy with improved factor combination
    // - High diversity + good improvement = lower mutation (exploitation)
    // - Low diversity + poor improvement = higher mutation (exploration)
    // - Stagnation (many generations without improvement) = increase mutation

    float explorationFactor = 1.0f;
    float stagnationFactor = 1.0f;
    float convergenceFactor = 1.0f;
    float diversityFactor = 1.0f;

    // Detect performance breakthrough (very high recent improvement)
    bool performanceBreakthrough = (metrics->avgScoreImprovement > 0.5f); // 50%+ improvement
    bool strongImprovement = (metrics->avgScoreImprovement > 0.1f);       // 10%+ improvement

    // Factor 1: Improvement-based adaptation (improved logic)
    if (performanceBreakthrough) {
        // Major breakthrough! Significantly reduce mutation to preserve good genes
        explorationFactor = 0.4f;
    } else if (strongImprovement && metrics->generationsSinceImprovement < 10) {
        // Strong recent improvement, reduce mutation moderately
        explorationFactor = 0.6f;
    } else if (metrics->avgScoreImprovement > SIGNIFICANT_IMPROVEMENT_THRESHOLD) {
        // Good improvement, reduce mutation (exploitation)
        explorationFactor = 0.8f;
    } else if (metrics->avgScoreImprovement < -SIGNIFICANT_IMPROVEMENT_THRESHOLD) {
        // Negative improvement, increase mutation (exploration)
        explorationFactor = 1.4f;
    }

    // Factor 2: Stagnation-based adaptation (but override for breakthroughs)
    if (performanceBreakthrough || (strongImprovement && metrics->generationsSinceImprovement < 5)) {
        // Recent breakthrough - ignore stagnation count, focus on exploitation
        stagnationFactor = 0.7f;
    } else if (metrics->generationsSinceImprovement > 300) {
        // Extreme stagnation - emergency mode
        stagnationFactor = 4.0f;
    } else if (metrics->generationsSinceImprovement > 150) {
        // Very long stagnation
        stagnationFactor = 2.5f + (metrics->generationsSinceImprovement - 150) * 0.01f;
        stagnationFactor = fminf(stagnationFactor, 4.0f);
    } else if (metrics->generationsSinceImprovement > 75) {
        // Long stagnation
        stagnationFactor = 1.8f + (metrics->generationsSinceImprovement - 75) * 0.01f;
    } else if (metrics->generationsSinceImprovement > 30) {
        // Moderate stagnation
        stagnationFactor = 1.2f + (metrics->generationsSinceImprovement - 30) * 0.013f;
    } else if (metrics->generationsSinceImprovement > 15) {
        // Some stagnation
        stagnationFactor = 1.1f;
    }

    // Factor 3: Convergence-based adaptation
    if (metrics->convergenceRate > 0.85f) {
        // Very high convergence
        convergenceFactor = 2.0f;
    } else if (metrics->convergenceRate > 0.65f) {
        // High convergence
        convergenceFactor = 1.5f;
    } else if (metrics->convergenceRate < 0.25f) {
        // Low convergence (high diversity)
        convergenceFactor = 0.9f;
    }

    // Factor 4: Diversity-based adaptation
    if (metrics->diversityIndex < 0.05f) {
        // Extremely low diversity - population collapsed
        diversityFactor = 3.5f;
    } else if (metrics->diversityIndex < 0.15f) {
        // Very low diversity
        diversityFactor = 2.5f;
    } else if (metrics->diversityIndex < 0.4f) {
        // Low diversity
        diversityFactor = 1.8f;
    } else if (metrics->diversityIndex > 0.8f) {
        // High diversity - reduce mutation
        diversityFactor = 0.8f;
    }

    // Improved factor combination - weighted average instead of multiplication
    // This prevents extreme amplification while maintaining responsiveness
    float weights[] = {0.2f, 0.4f, 0.25f, 0.15f}; // Stagnation gets highest weight
    float mutationMultiplier = (explorationFactor * weights[0] +
                               stagnationFactor * weights[1] +
                               convergenceFactor * weights[2] +
                               diversityFactor * weights[3]);

    // Emergency "evolutionary shock" for extreme stagnation
    if (metrics->generationsSinceImprovement > 200) {
        mutationMultiplier = fmaxf(mutationMultiplier, 3.5f);
    }

    // Apply adaptation to reset mutations (used between generations)
    params->resetMutationRate = NEURAL_NETWORK_RESET_MUTATION_RATE * mutationMultiplier;
    params->resetMutationProb = NEURAL_NETWORK_RESET_MUTATION_PROB * mutationMultiplier;

    // Apply smaller adaptation to child mutations (used during reproduction)
    float childMultiplier = 1.0f + (mutationMultiplier - 1.0f) * 0.6f; // 60% of effect
    params->childMutationRate = NEURAL_NETWORK_CHILD_MUTATION_RATE * childMultiplier;
    params->childMutationProb = NEURAL_NETWORK_CHILD_MUTATION_PROB * childMultiplier;

    // Clamp values to reasonable bounds
    params->resetMutationRate = fmaxf(MIN_MUTATION_RATE,
                                     fminf(MAX_MUTATION_RATE, params->resetMutationRate));
    params->resetMutationProb = fmaxf(MIN_MUTATION_PROB,
                                     fminf(MAX_MUTATION_PROB, params->resetMutationProb));
    params->childMutationRate = fmaxf(MIN_MUTATION_RATE,
                                     fminf(MAX_MUTATION_RATE, params->childMutationRate));
    params->childMutationProb = fmaxf(MIN_MUTATION_PROB,
                                     fminf(MAX_MUTATION_PROB, params->childMutationProb));
}
