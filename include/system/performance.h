/**
 * @file performance.h
 * @brief Simple performance measurement system for BipLab
 */

#ifndef PERFORMANCE_H
#define PERFORMANCE_H

#include <time.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    PERF_NEURAL_NETWORK,
    PERF_CELL_UPDATE,
    PERF_MUTATION
} PerfTimerType;

typedef struct {
    double totalTime;
    double minTime;
    double maxTime;
    double avgTime;
    uint64_t callCount;
    double throughput;
} PerfStats;

bool Perf_Init(bool enableLogging, const char *logFilePath);
void Perf_Cleanup(void);
int Perf_CreateTimer(PerfTimerType type);
bool Perf_StartTimer(PerfTimerType type);
bool Perf_StartTimerWithOps(PerfTimerType type, uint64_t operationCount);
double Perf_EndTimer(PerfTimerType type);
const PerfStats* Perf_GetStats(PerfTimerType type);

#define PERF_MEASURE(timer_type) \
    for (int _perf_loop = (Perf_StartTimer(timer_type), 1); \
         _perf_loop; \
         _perf_loop = 0, Perf_EndTimer(timer_type))

#endif // PERFORMANCE_H
