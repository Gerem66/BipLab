/**
 * @file gpu_utils.h
 * @brief GPU name cleaning and identification utilities
 */

#ifndef GPU_UTILS_H
#define GPU_UTILS_H

#include <stddef.h>
#include <stdbool.h>

/**
 * Clean and simplify GPU name from raw hardware information
 * Converts verbose hardware names into concise, user-friendly format
 *
 * @param rawName Raw GPU name from lspci or other hardware detection
 * @param cleanName Output buffer for cleaned name
 * @param maxLen Maximum length of output buffer
 *
 * Examples:
 * Input:  "Advanced Micro Devices, Inc. [AMD/ATI] Navi 22 [Radeon RX 6700/6700 XT/6750 XT / 6800M/6850M XT]"
 * Output: "AMD Radeon RX 6700/6800 XT"
 *
 * Input:  "NVIDIA Corporation GA104 [GeForce RTX 3060 Ti GDDR6X] (rev a1)"
 * Output: "NVIDIA GeForce RTX 3060 Ti"
 */
void GPU_CleanName(const char* rawName, char* cleanName, size_t maxLen);

/**
 * Get GPU name from vendor and device IDs
 * Used when detailed hardware names are not available
 *
 * @param vendorId Vendor ID (e.g., 0x1002 for AMD, 0x10de for NVIDIA)
 * @param deviceId Device ID for specific GPU model
 * @param gpuName Output buffer for GPU name
 * @param maxLen Maximum length of output buffer
 * @return true if GPU was identified, false for generic fallback
 */
bool GPU_GetNameFromIds(unsigned int vendorId, unsigned int deviceId, char* gpuName, size_t maxLen);

#endif // GPU_UTILS_H
