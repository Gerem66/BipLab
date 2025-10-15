/**
 * @file hardware_monitor.c
 * @brief Implementation of the hardware monitoring system
 *
 * This implementation uses Linux-specific methods to gather hardware information
 * and real-time usage statistics. For cross-platform support, platform-specific
 * implementations would be needed.
 */

#include "../../include/system/hardware_monitor.h"
#include "../../include/system/gpu_utils.h"
#include "../../include/core/string_utils.h"
#include "../../include/ui/components/realtime_graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/sysinfo.h>

// Global hardware monitor instance
static HardwareMonitor g_hwMonitor = {0};

// Static CPU time tracking for usage calculation
static struct {
    unsigned long long prevUser, prevNice, prevSystem, prevIdle;
    unsigned long long prevIowait, prevIrq, prevSoftirq, prevSteal;
    bool hasValidPrevious;
} g_cpuTimes = {0};

// Helper function prototypes
static bool ReadCPUInfo(CPUInfo* cpuInfo);
static bool ReadGPUInfo(GPUInfo* gpuInfo);
static bool ReadRAMInfo(RAMInfo* ramInfo);
static double CalculateCPUUsage(void);
static double CalculateRAMUsage(void);
static double CalculateGPUUsage(void);

void HwMonitor_Init(void)
{
    memset(&g_hwMonitor, 0, sizeof(HardwareMonitor));

    // Initialize CPU information
    if (!ReadCPUInfo(&g_hwMonitor.cpu)) {
        fprintf(stderr, "Warning: Failed to read CPU information\n");
    }

    // Initialize RAM information
    if (!ReadRAMInfo(&g_hwMonitor.ram)) {
        fprintf(stderr, "Warning: Failed to read RAM information\n");
    }

    // Initialize GPU information (optional)
    g_hwMonitor.gpuMonitoringEnabled = true;
    if (!ReadGPUInfo(&g_hwMonitor.gpu)) {
        g_hwMonitor.gpuMonitoringEnabled = false;
    }

    // Initialize current usage values with initial readings
    g_hwMonitor.usage.currentCPU = CalculateCPUUsage();
    g_hwMonitor.usage.currentRAM = CalculateRAMUsage();
    g_hwMonitor.usage.currentGPU = g_hwMonitor.gpuMonitoringEnabled ? CalculateGPUUsage() : 0.0;

    g_hwMonitor.isInitialized = true;
    g_hwMonitor.lastUpdateFrame = 0;
}

void HwMonitor_Cleanup(void)
{
    memset(&g_hwMonitor, 0, sizeof(HardwareMonitor));
    memset(&g_cpuTimes, 0, sizeof(g_cpuTimes));
}

HardwareMonitor* HwMonitor_Get(void)
{
    return g_hwMonitor.isInitialized ? &g_hwMonitor : NULL;
}

bool HwMonitor_Update(int frameNumber)
{
    if (!g_hwMonitor.isInitialized) {
        return false;
    }

    // Check if we need to update (based on real time, not frames)
    static struct timespec lastUpdateTime = {0, 0};
    struct timespec currentTime;

    clock_gettime(CLOCK_MONOTONIC, &currentTime);

    double timeSinceLastUpdate = (currentTime.tv_sec - lastUpdateTime.tv_sec) + 
                                (currentTime.tv_nsec - lastUpdateTime.tv_nsec) / 1e9;

    if (timeSinceLastUpdate < GRAPH_UPDATE_INTERVAL) {
        return false;
    }
    lastUpdateTime = currentTime;

    clock_t startTime = clock();

    // Update RAM info (this changes dynamically)
    ReadRAMInfo(&g_hwMonitor.ram);

    // Calculate current usage percentages
    g_hwMonitor.usage.currentCPU = CalculateCPUUsage();
    g_hwMonitor.usage.currentRAM = CalculateRAMUsage();
    g_hwMonitor.usage.currentGPU = g_hwMonitor.gpuMonitoringEnabled ? CalculateGPUUsage() : 0.0;

    // Update performance metrics
    clock_t endTime = clock();
    g_hwMonitor.updateTimeMs = ((double)(endTime - startTime)) / CLOCKS_PER_SEC * 1000.0;
    g_hwMonitor.updateCount++;
    g_hwMonitor.lastUpdateFrame = frameNumber;

    return true;
}

// Getter functions
const CPUInfo* HwMonitor_GetCPUInfo(void) {
    return g_hwMonitor.isInitialized ? &g_hwMonitor.cpu : NULL;
}

const GPUInfo* HwMonitor_GetGPUInfo(void) {
    return g_hwMonitor.isInitialized ? &g_hwMonitor.gpu : NULL;
}

const RAMInfo* HwMonitor_GetRAMInfo(void) {
    return g_hwMonitor.isInitialized ? &g_hwMonitor.ram : NULL;
}

double HwMonitor_GetCurrentCPUUsage(void) {
    return g_hwMonitor.isInitialized ? g_hwMonitor.usage.currentCPU : 0.0;
}

double HwMonitor_GetCurrentRAMUsage(void) {
    return g_hwMonitor.isInitialized ? g_hwMonitor.usage.currentRAM : 0.0;
}

double HwMonitor_GetCurrentGPUUsage(void) {
    return g_hwMonitor.isInitialized ? g_hwMonitor.usage.currentGPU : 0.0;
}

void HwMonitor_SetGPUMonitoring(bool enabled) {
    g_hwMonitor.gpuMonitoringEnabled = enabled && g_hwMonitor.gpu.isAvailable;
}

// Helper function implementations

static bool ReadCPUInfo(CPUInfo* cpuInfo)
{
    FILE* fp = fopen("/proc/cpuinfo", "r");
    if (!fp) {
        return false;
    }

    char line[256];
    bool foundModel = false;
    int cores = 0, threads = 0;
    double maxFreq = 0.0;

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "model name", 10) == 0 && !foundModel) {
            char* colon = strchr(line, ':');
            if (colon) {
                String_SafeCopy(cpuInfo->model, HW_MAX_NAME_LENGTH, String_Trim(colon + 1));
                foundModel = true;
            }
        } else if (strncmp(line, "processor", 9) == 0) {
            threads++;
        } else if (strncmp(line, "cpu cores", 9) == 0) {
            char* colon = strchr(line, ':');
            if (colon) {
                cores = atoi(colon + 1);
            }
        } else if (strncmp(line, "cpu MHz", 7) == 0) {
            char* colon = strchr(line, ':');
            if (colon) {
                double freq = atof(colon + 1) / 1000.0;
                if (freq > maxFreq) maxFreq = freq;
            }
        }
    }

    fclose(fp);

    cpuInfo->coreCount = cores > 0 ? cores : threads; // Fallback to thread count
    cpuInfo->threadCount = threads;
    cpuInfo->maxFrequency = maxFreq;
    cpuInfo->baseFrequency = maxFreq * 0.8; // Rough estimate

    return foundModel;
}

static bool ReadGPUInfo(GPUInfo* gpuInfo)
{
    // Check if we're in a toolbox environment
    bool inToolbox = getenv("TOOLBOX_PATH") != NULL || getenv("container") != NULL;

    // Method 1: Try lspci first (if available)
    FILE* fp = popen("command -v lspci >/dev/null 2>&1 && lspci 2>/dev/null | grep -i 'vga\\|3d\\|display'", "r");
    if (fp) {
        char line[256];
        char discreteGPU[256] = {0};
        char integratedGPU[256] = {0};
        bool foundDiscrete = false;

        // Scan all GPUs and prioritize discrete ones
        while (fgets(line, sizeof(line), fp)) {
            bool isAMD = strstr(line, "AMD") || strstr(line, "Radeon");
            bool isNVIDIA = strstr(line, "NVIDIA") || strstr(line, "GeForce");
            bool isIntel = strstr(line, "Intel");

            if (isAMD || isNVIDIA) {
                // Found discrete GPU - priority
                strncpy(discreteGPU, line, sizeof(discreteGPU) - 1);
                foundDiscrete = true;
            } else if (isIntel && !foundDiscrete) {
                // Intel integrated as fallback
                strncpy(integratedGPU, line, sizeof(integratedGPU) - 1);
            }
        }
        pclose(fp);

        // Use discrete GPU if found, otherwise integrated
        char* selectedGPU = foundDiscrete ? discreteGPU : integratedGPU;
        if (strlen(selectedGPU) > 0) {
            char* colon = strrchr(selectedGPU, ':');
            if (colon) {
                // Trim manually inline
                char* start = colon + 1;
                while (*start == ' ' || *start == '\t') start++;
                char* end = start + strlen(start) - 1;
                while (end > start && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
                    *end = '\0';
                    end--;
                }

                // Clean the GPU name to make it more concise
                GPU_CleanName(start, gpuInfo->model, HW_MAX_NAME_LENGTH);
                gpuInfo->isAvailable = true;
                gpuInfo->isDiscrete = foundDiscrete;
            }
        }
    }

    // Method 2: If lspci failed, use DRM filesystem (works in toolbox)
    if (!gpuInfo->isAvailable) {
        fp = popen("find /sys/class/drm/card*/device -name vendor 2>/dev/null", "r");
        if (fp) {
            char vendorPath[256];
            char discreteGPU[256] = {0};
            char integratedGPU[256] = {0};
            bool foundDiscrete = false;

            // Check each graphics card
            while (fgets(vendorPath, sizeof(vendorPath), fp)) {
                vendorPath[strcspn(vendorPath, "\n")] = 0; // Remove newline

                FILE* vendor_fp = fopen(vendorPath, "r");
                if (vendor_fp) {
                    char vendor[16];
                    if (fgets(vendor, sizeof(vendor), vendor_fp)) {
                        vendor[strcspn(vendor, "\n")] = 0;

                        // Also try to get device ID for more specific identification
                        char devicePath[512];
                        strcpy(devicePath, vendorPath);
                        char* lastSlash = strrchr(devicePath, '/');
                        if (lastSlash) {
                            strcpy(lastSlash + 1, "device");
                        }

                        char device[16] = {0};
                        FILE* device_fp = fopen(devicePath, "r");
                        if (device_fp) {
                            fgets(device, sizeof(device), device_fp);
                            device[strcspn(device, "\n")] = 0;
                            fclose(device_fp);
                        }

                        // Check vendor ID and device ID for specific models
                        unsigned int vendorId = (unsigned int)strtoul(vendor, NULL, 16);
                        unsigned int deviceId = (unsigned int)strtoul(device, NULL, 16);

                        if (vendorId == 0x1002) { // AMD
                            GPU_GetNameFromIds(vendorId, deviceId, discreteGPU, sizeof(discreteGPU));
                            foundDiscrete = true;
                        } else if (vendorId == 0x10de) { // NVIDIA
                            GPU_GetNameFromIds(vendorId, deviceId, discreteGPU, sizeof(discreteGPU));
                            foundDiscrete = true;
                        } else if (vendorId == 0x8086 && !foundDiscrete) { // Intel fallback
                            GPU_GetNameFromIds(vendorId, deviceId, integratedGPU, sizeof(integratedGPU));
                        }
                    }
                    fclose(vendor_fp);
                }
            }
            pclose(fp);

            // Use discrete GPU if found, otherwise integrated
            char* selectedGPU = foundDiscrete ? discreteGPU : integratedGPU;
            if (strlen(selectedGPU) > 0) {
                strncpy(gpuInfo->model, selectedGPU, HW_MAX_NAME_LENGTH - 1);
                gpuInfo->model[HW_MAX_NAME_LENGTH - 1] = '\0';
                gpuInfo->isAvailable = true;
                gpuInfo->isDiscrete = foundDiscrete;
            }
        }
    }

    // If lspci failed, try alternative methods
    if (!gpuInfo->isAvailable) {
        // Try reading from /sys/class/drm/ (Linux DRM subsystem)
        fp = popen("find /sys/class/drm/card*/device -name vendor 2>/dev/null | head -1", "r");
        if (fp) {
            char vendorPath[256];
            if (fgets(vendorPath, sizeof(vendorPath), fp)) {
                // We found a graphics card, set info based on environment
                if (inToolbox) {
                    strcpy(gpuInfo->model, "Host GPU (toolbox)");
                } else {
                    strcpy(gpuInfo->model, "Graphics Device (detected)");
                }
                gpuInfo->isAvailable = true;
                gpuInfo->isDiscrete = false; // Assume integrated by default
            }
            pclose(fp);
        }
    }

    // Special handling for toolbox environments
    if (!gpuInfo->isAvailable && inToolbox) {
        strcpy(gpuInfo->model, "Host GPU (limited access)");
        gpuInfo->isAvailable = false; // Keep as false to avoid showing usage
        gpuInfo->isDiscrete = false;
    }

    // Try to get GPU memory info
    if (gpuInfo->isAvailable && strstr(gpuInfo->model, "NVIDIA")) {
        // NVIDIA GPU memory detection
        fp = popen("nvidia-smi --query-gpu=memory.total --format=csv,noheader,nounits 2>/dev/null", "r");
        if (fp) {
            char line[64];
            if (fgets(line, sizeof(line), fp)) {
                gpuInfo->totalMemory = (uint64_t)atol(line) * 1024 * 1024; // Convert MB to bytes
            }
            pclose(fp);
        }
    } else if (gpuInfo->isAvailable && (strstr(gpuInfo->model, "AMD") || strstr(gpuInfo->model, "Radeon"))) {
        // AMD GPU memory detection via DRM
        fp = popen("find /sys/class/drm/card*/device -name mem_info_vram_total 2>/dev/null | head -1", "r");
        if (fp) {
            char path[256];
            if (fgets(path, sizeof(path), fp)) {
                pclose(fp);
                // Remove newline
                path[strcspn(path, "\n")] = 0;

                // Read VRAM size
                FILE* vram_fp = fopen(path, "r");
                if (vram_fp) {
                    char vram_line[64];
                    if (fgets(vram_line, sizeof(vram_line), vram_fp)) {
                        gpuInfo->totalMemory = (uint64_t)atoll(vram_line); // Already in bytes
                    }
                    fclose(vram_fp);
                }
            } else {
                pclose(fp);
            }
        }

        // Alternative method for AMD: try lspci for memory info
        if (gpuInfo->totalMemory == 0) {
            fp = popen("lspci -v 2>/dev/null | grep -A 10 'AMD\\|Radeon' | grep 'Memory.*prefetchable' | head -1", "r");
            if (fp) {
                char line[256];
                if (fgets(line, sizeof(line), fp)) {
                    // Extract memory size from lspci output (format: Memory at ... [size=XM])
                    char* size_str = strstr(line, "size=");
                    if (size_str) {
                        size_str += 5; // Skip "size="
                        long size_mb = atol(size_str);
                        if (size_mb > 0) {
                            gpuInfo->totalMemory = size_mb * 1024 * 1024; // Convert MB to bytes
                        }
                    }
                }
                pclose(fp);
            }
        }
    }

    return gpuInfo->isAvailable;
}

static bool ReadRAMInfo(RAMInfo* ramInfo)
{
    FILE* fp = fopen("/proc/meminfo", "r");
    if (!fp) {
        return false;
    }

    char line[256];
    uint64_t memTotal = 0, memFree = 0, memAvailable = 0;
    uint64_t buffers = 0, cached = 0;

    while (fgets(line, sizeof(line), fp)) {
        unsigned long temp;
        if (sscanf(line, "MemTotal: %lu kB", &temp) == 1) {
            memTotal = temp * 1024; // Convert kB to bytes
        } else if (sscanf(line, "MemFree: %lu kB", &temp) == 1) {
            memFree = temp * 1024;
        } else if (sscanf(line, "MemAvailable: %lu kB", &temp) == 1) {
            memAvailable = temp * 1024;
        } else if (sscanf(line, "Buffers: %lu kB", &temp) == 1) {
            buffers = temp * 1024;
        } else if (sscanf(line, "Cached: %lu kB", &temp) == 1) {
            cached = temp * 1024;
        }
    }
    fclose(fp);

    if (memTotal == 0) {
        return false;
    }

    ramInfo->totalRAM = memTotal;
    // Use MemAvailable if available (more accurate), otherwise calculate
    ramInfo->availableRAM = memAvailable > 0 ? memAvailable : (memFree + buffers + cached);
    ramInfo->usedRAM = ramInfo->totalRAM - ramInfo->availableRAM;
    ramInfo->usagePercent = ((double)ramInfo->usedRAM / (double)ramInfo->totalRAM) * 100.0;

    return true;
}

static double CalculateCPUUsage(void)
{
    FILE* fp = fopen("/proc/stat", "r");
    if (!fp) {
        return 0.0;
    }

    char line[256];
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return 0.0;
    }
    fclose(fp);

    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    sscanf(line, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
           &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);

    if (!g_cpuTimes.hasValidPrevious) {
        // First read, just store values
        g_cpuTimes.prevUser = user;
        g_cpuTimes.prevNice = nice;
        g_cpuTimes.prevSystem = system;
        g_cpuTimes.prevIdle = idle;
        g_cpuTimes.prevIowait = iowait;
        g_cpuTimes.prevIrq = irq;
        g_cpuTimes.prevSoftirq = softirq;
        g_cpuTimes.prevSteal = steal;
        g_cpuTimes.hasValidPrevious = true;
        return 0.0;
    }

    // Calculate deltas
    unsigned long long totalDelta = (user - g_cpuTimes.prevUser) +
                                   (nice - g_cpuTimes.prevNice) +
                                   (system - g_cpuTimes.prevSystem) +
                                   (idle - g_cpuTimes.prevIdle) +
                                   (iowait - g_cpuTimes.prevIowait) +
                                   (irq - g_cpuTimes.prevIrq) +
                                   (softirq - g_cpuTimes.prevSoftirq) +
                                   (steal - g_cpuTimes.prevSteal);

    unsigned long long idleDelta = (idle - g_cpuTimes.prevIdle) +
                                  (iowait - g_cpuTimes.prevIowait);

    double usage = 0.0;
    if (totalDelta > 0) {
        usage = ((double)(totalDelta - idleDelta) / (double)totalDelta) * 100.0;
    }

    // Update previous values
    g_cpuTimes.prevUser = user;
    g_cpuTimes.prevNice = nice;
    g_cpuTimes.prevSystem = system;
    g_cpuTimes.prevIdle = idle;
    g_cpuTimes.prevIowait = iowait;
    g_cpuTimes.prevIrq = irq;
    g_cpuTimes.prevSoftirq = softirq;
    g_cpuTimes.prevSteal = steal;

    return usage;
}

static double CalculateRAMUsage(void)
{
    return g_hwMonitor.ram.usagePercent;
}

static double CalculateGPUUsage(void)
{
    // GPU usage monitoring is complex and GPU-specific
    // For now, return a placeholder (would need nvidia-ml-py or similar)
    if (!g_hwMonitor.gpu.isAvailable || !strstr(g_hwMonitor.gpu.model, "NVIDIA")) {
        return 0.0;
    }

    FILE* fp = popen("nvidia-smi --query-gpu=utilization.gpu --format=csv,noheader,nounits", "r");
    if (fp) {
        char line[64];
        if (fgets(line, sizeof(line), fp)) {
            pclose(fp);
            return atof(line);
        }
        pclose(fp);
    }

    return 0.0;
}

char* HwMonitor_FormatMemorySize(uint64_t bytes, char* buffer)
{
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unitIndex = 0;
    double size = (double)bytes;

    while (size >= 1024.0 && unitIndex < 4) {
        size /= 1024.0;
        unitIndex++;
    }

    if (unitIndex == 0) {
        sprintf(buffer, "%.0f %s", size, units[unitIndex]);
    } else {
        sprintf(buffer, "%.1f %s", size, units[unitIndex]);
    }

    return buffer;
}

char* HwMonitor_FormatFrequency(double ghz, char* buffer)
{
    if (ghz >= 1.0) {
        sprintf(buffer, "%.2f GHz", ghz);
    } else {
        sprintf(buffer, "%.0f MHz", ghz * 1000.0);
    }
    return buffer;
}
