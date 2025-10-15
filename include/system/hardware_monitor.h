/**
 * @file hardware_monitor.h
 * @brief Hardware monitoring system for real-time system information
 *
 * This module provides comprehensive hardware monitoring capabilities
 * including CPU, RAM, and GPU information with historical data tracking
 * for real-time visualization in the training interface.
 */

#ifndef HARDWARE_MONITOR_H
#define HARDWARE_MONITOR_H

#include <stdbool.h>
#include <stdint.h>

// Configuration constants
#define HW_MAX_NAME_LENGTH 256          // Maximum length for hardware names

// Hardware information structures
typedef struct {
    char model[HW_MAX_NAME_LENGTH];     // CPU model name
    int coreCount;                      // Number of CPU cores
    int threadCount;                    // Number of logical threads
    double baseFrequency;               // Base frequency in GHz
    double maxFrequency;                // Max frequency in GHz
} CPUInfo;

typedef struct {
    char model[HW_MAX_NAME_LENGTH];     // GPU model name
    uint64_t totalMemory;               // Total GPU memory in bytes
    bool isAvailable;                   // Whether GPU monitoring is available
    bool isDiscrete;                    // True for discrete GPU, false for integrated
} GPUInfo;

typedef struct {
    uint64_t totalRAM;                  // Total system RAM in bytes
    uint64_t availableRAM;              // Available RAM in bytes
    uint64_t usedRAM;                   // Used RAM in bytes
    double usagePercent;                // RAM usage percentage (0.0-100.0)
} RAMInfo;

// Real-time usage data
typedef struct {
    double currentCPU;                           // Current CPU usage
    double currentRAM;                           // Current RAM usage
    double currentGPU;                           // Current GPU usage
} UsageData;

// Main hardware monitor structure
typedef struct {
    CPUInfo cpu;                        // CPU information
    GPUInfo gpu;                        // GPU information
    RAMInfo ram;                        // RAM information
    UsageData usage;                    // Current usage data

    bool isInitialized;                 // Whether the monitor is initialized
    int lastUpdateFrame;                // Last frame when data was updated
    bool gpuMonitoringEnabled;          // Whether GPU monitoring is enabled

    // Performance metrics
    double updateTimeMs;                // Time taken for last update in ms
    uint64_t updateCount;               // Total number of updates performed
} HardwareMonitor;

// Function declarations

/**
 * Initialize the hardware monitoring system
 * @return true if initialization was successful
 */
void HwMonitor_Init(void);

/**
 * Cleanup and shutdown the hardware monitoring system
 */
void HwMonitor_Cleanup(void);

/**
 * Get the global hardware monitor instance
 * @return Pointer to the hardware monitor, or NULL if not initialized
 */
HardwareMonitor* HwMonitor_Get(void);

/**
 * Update hardware usage data (call this periodically)
 * @param frameNumber Current frame number for update timing
 * @return true if data was actually updated
 */
bool HwMonitor_Update(int frameNumber);

/**
 * Get static hardware information (CPU model, RAM size, etc.)
 * This information is gathered once at initialization
 */
const CPUInfo* HwMonitor_GetCPUInfo(void);
const GPUInfo* HwMonitor_GetGPUInfo(void);
const RAMInfo* HwMonitor_GetRAMInfo(void);

/**
 * Get current usage percentages
 */
double HwMonitor_GetCurrentCPUUsage(void);
double HwMonitor_GetCurrentRAMUsage(void);
double HwMonitor_GetCurrentGPUUsage(void);

/**
 * Enable/disable GPU monitoring (can be expensive on some systems)
 * @param enabled Whether to enable GPU monitoring
 */
void HwMonitor_SetGPUMonitoring(bool enabled);

/**
 * Get a human-readable string for memory size
 * @param bytes Memory size in bytes
 * @param buffer Output buffer (must be at least 32 bytes)
 * @return Pointer to the buffer
 */
char* HwMonitor_FormatMemorySize(uint64_t bytes, char* buffer);

/**
 * Get a human-readable string for frequency
 * @param ghz Frequency in GHz
 * @param buffer Output buffer (must be at least 16 bytes)
 * @return Pointer to the buffer
 */
char* HwMonitor_FormatFrequency(double ghz, char* buffer);

#endif // HARDWARE_MONITOR_H
