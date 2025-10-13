/**
 * @file performance.c
 * @brief Simple implementation of the performance system
 */

#include "../../include/system/performance.h"
#include <stdio.h> // FILE, fopen, fclose
#include <string.h> // memset
#include <float.h> // DBL_MAX;

#define MAX_TIMERS 16
#define MAX_SAMPLES 1000

typedef struct {
    PerfTimerType type;
    bool isActive;
    bool isRunning;
    struct timespec startTime;
    double samples[MAX_SAMPLES];
    int sampleIndex;
    int sampleCount;
    PerfStats stats;
    uint64_t operationCount;
} PerfTimer;

static struct {
    PerfTimer timers[MAX_TIMERS];
    int timerCount;
    bool isEnabled;
    FILE *logHandle;
} g_perf = {0};

static PerfTimer* FindTimer(PerfTimerType type) {
    for (int i = 0; i < g_perf.timerCount; i++) {
        if (g_perf.timers[i].type == type && g_perf.timers[i].isActive) {
            return &g_perf.timers[i];
        }
    }
    return NULL;
}

static void CalculateStats(PerfTimer *timer) {
    if (timer->sampleCount == 0) return;

    timer->stats.totalTime = 0.0;
    timer->stats.minTime = DBL_MAX;
    timer->stats.maxTime = 0.0;

    for (int i = 0; i < timer->sampleCount; i++) {
        double sample = timer->samples[i];
        timer->stats.totalTime += sample;
        if (sample < timer->stats.minTime) timer->stats.minTime = sample;
        if (sample > timer->stats.maxTime) timer->stats.maxTime = sample;
    }

    timer->stats.avgTime = timer->stats.totalTime / timer->sampleCount;

    if (timer->operationCount > 0 && timer->stats.avgTime > 0) {
        timer->stats.throughput = (double)timer->operationCount / (timer->stats.avgTime / 1000000.0);
    }
}

bool Perf_Init(bool enableLogging, const char *logFilePath) {
    memset(&g_perf, 0, sizeof(g_perf));
    g_perf.isEnabled = true;

    if (enableLogging && logFilePath) {
        g_perf.logHandle = fopen(logFilePath, "w");
    }

    return true;
}

void Perf_Cleanup(void) {
    if (g_perf.logHandle) {
        fclose(g_perf.logHandle);
    }
    memset(&g_perf, 0, sizeof(g_perf));
}

int Perf_CreateTimer(PerfTimerType type) {
    if (g_perf.timerCount >= MAX_TIMERS) return -1;

    PerfTimer *timer = &g_perf.timers[g_perf.timerCount];
    timer->type = type;
    timer->isActive = true;
    timer->stats.minTime = DBL_MAX;

    return g_perf.timerCount++;
}

bool Perf_StartTimer(PerfTimerType type) {
    return Perf_StartTimerWithOps(type, 1);
}

bool Perf_StartTimerWithOps(PerfTimerType type, uint64_t operationCount) {
    if (!g_perf.isEnabled) return false;

    PerfTimer *timer = FindTimer(type);
    if (!timer) {
        int id = Perf_CreateTimer(type);
        if (id < 0) return false;
        timer = &g_perf.timers[id];
    }

    if (timer->isRunning) return false;

    timer->operationCount = operationCount;
    clock_gettime(CLOCK_MONOTONIC, &timer->startTime);
    timer->isRunning = true;

    return true;
}

double Perf_EndTimer(PerfTimerType type) {
    if (!g_perf.isEnabled) return -1.0;

    PerfTimer *timer = FindTimer(type);
    if (!timer || !timer->isRunning) return -1.0;

    struct timespec endTime;
    clock_gettime(CLOCK_MONOTONIC, &endTime);
    timer->isRunning = false;

    double elapsed = (double)(endTime.tv_sec - timer->startTime.tv_sec) * 1000000.0 +
                    (double)(endTime.tv_nsec - timer->startTime.tv_nsec) / 1000.0;

    if (timer->sampleCount < MAX_SAMPLES) {
        timer->samples[timer->sampleCount++] = elapsed;
    } else {
        timer->samples[timer->sampleIndex] = elapsed;
        timer->sampleIndex = (timer->sampleIndex + 1) % MAX_SAMPLES;
    }

    timer->stats.callCount++;
    CalculateStats(timer);

    return elapsed;
}

const PerfStats* Perf_GetStats(PerfTimerType type) {
    PerfTimer *timer = FindTimer(type);
    return timer ? &timer->stats : NULL;
}
