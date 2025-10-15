/**
 * @file gpu_utils.c
 * @brief Implementation of GPU name cleaning and identification utilities
 */

#include "../../include/system/gpu_utils.h"
#include "../../include/core/string_utils.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct { const char* needle; const char* label; } Pattern;

static const Pattern AMD_PATTERNS[] = {
    {"RX 6750", "AMD Radeon RX 6750 XT"},
    {"RX 6700", "AMD Radeon RX 6700 / 6750 XT"},
    {"RX 6800", "AMD Radeon RX 6800 / 6800 XT"},
    {"RX 6900", "AMD Radeon RX 6900 XT"},
    {"RX 6600", "AMD Radeon RX 6600 / 6650 XT"},
    {"RX 7900", "AMD Radeon RX 7900 XT/XTX"},
    {"RX 7800", "AMD Radeon RX 7800 XT"},
    {"RX 7700", "AMD Radeon RX 7700 XT"},
};

static const Pattern NVIDIA_PATTERNS[] = {
    {"RTX 4090", "NVIDIA GeForce RTX 4090"},
    {"RTX 4080", "NVIDIA GeForce RTX 4080"},
    {"RTX 4070 Ti", "NVIDIA GeForce RTX 4070 Ti"},
    {"RTX 4070", "NVIDIA GeForce RTX 4070"},
    {"RTX 4060 Ti", "NVIDIA GeForce RTX 4060 Ti"},
    {"RTX 4060", "NVIDIA GeForce RTX 4060"},
    {"RTX 3090 Ti", "NVIDIA GeForce RTX 3090 Ti"},
    {"RTX 3090", "NVIDIA GeForce RTX 3090"},
    {"RTX 3080 Ti", "NVIDIA GeForce RTX 3080 Ti"},
    {"RTX 3080", "NVIDIA GeForce RTX 3080"},
    {"RTX 3070 Ti", "NVIDIA GeForce RTX 3070 Ti"},
    {"RTX 3070", "NVIDIA GeForce RTX 3070"},
    {"RTX 3060 Ti", "NVIDIA GeForce RTX 3060 Ti"},
    {"RTX 3060", "NVIDIA GeForce RTX 3060"},
};

static const Pattern INTEL_PATTERNS[] = {
    {"UHD Graphics 770", "Intel UHD Graphics 770"},
    {"UHD Graphics 730", "Intel UHD Graphics 730"},
    {"Iris Xe",         "Intel Iris Xe Graphics"},
    {"Iris",            "Intel Iris Graphics"},
    {"UHD Graphics",    "Intel UHD Graphics"},
};

static bool match_any(const char* raw, const Pattern* table, size_t n, char* out, size_t maxLen){
    for (size_t i=0;i<n;i++){
        if (String_CaseInsensitiveSearch(raw, table[i].needle)) {
            String_SafeCopy(out, maxLen, table[i].label);
            return true;
        }
    }
    return false;
}

void GPU_CleanName(const char* rawName, char* cleanName, size_t maxLen)
{
    if (!rawName || !cleanName || maxLen == 0){ return; }

    if (String_CaseInsensitiveSearch(rawName, "AMD") || String_CaseInsensitiveSearch(rawName, "Radeon")){
        if (match_any(rawName, AMD_PATTERNS, sizeof(AMD_PATTERNS)/sizeof(AMD_PATTERNS[0]), cleanName, maxLen)) return;
        String_SafeCopy(cleanName, maxLen, "AMD Radeon Graphics");
        return;
    }
    if (String_CaseInsensitiveSearch(rawName, "NVIDIA") || String_CaseInsensitiveSearch(rawName, "GeForce")){
        if (match_any(rawName, NVIDIA_PATTERNS, sizeof(NVIDIA_PATTERNS)/sizeof(NVIDIA_PATTERNS[0]), cleanName, maxLen)) return;
        String_SafeCopy(cleanName, maxLen, "NVIDIA GeForce Graphics");
        return;
    }
    if (String_CaseInsensitiveSearch(rawName, "Intel")){
        if (match_any(rawName, INTEL_PATTERNS, sizeof(INTEL_PATTERNS)/sizeof(INTEL_PATTERNS[0]), cleanName, maxLen)) return;
        String_SafeCopy(cleanName, maxLen, "Intel Graphics");
        return;
    }

    // Fallback: tronque proprement
    String_SafeCopy(cleanName, maxLen, rawName);
}

typedef struct { unsigned vendor, device; const char* name; } PciId;

static const PciId PCI_TABLE[] = {
    // AMD RDNA2 (0x1002)
    {0x1002, 0x73DF, "AMD Radeon RX 6700/6750 XT"}, // Navi 22
    {0x1002, 0x73FF, "AMD Radeon RX 6600/6650 XT"},         // Navi 23
    {0x1002, 0x73BF, "AMD Radeon RX 6900 XT"},              // Navi 21
    {0x1002, 0x73A3, "AMD Radeon RX 6950 XT"},
    // AMD RDNA3
    {0x1002, 0x744C, "AMD Radeon RX 7900 XTX"},             // Navi 31
    {0x1002, 0x7448, "AMD Radeon RX 7900 XT"},

    // NVIDIA Ampere/Ada (0x10DE)
    {0x10DE, 0x2489, "NVIDIA GeForce RTX 3060 Ti GDDR6X"},  // GA104
    {0x10DE, 0x2484, "NVIDIA GeForce RTX 3070"},
    {0x10DE, 0x2206, "NVIDIA GeForce RTX 3080"},
    {0x10DE, 0x2204, "NVIDIA GeForce RTX 3090"},
    {0x10DE, 0x2782, "NVIDIA GeForce RTX 4070"},
    {0x10DE, 0x2783, "NVIDIA GeForce RTX 4070 Ti"},

    // Intel
    {0x8086, 0xA780, "Intel UHD Graphics 770"},
    {0x8086, 0xA7A0, "Intel UHD Graphics 730"},
    {0x8086, 0x9A49, "Intel Iris Xe Graphics"},
};

bool GPU_GetNameFromIds(unsigned int vendorId, unsigned int deviceId, char* gpuName, size_t maxLen)
{
    if (!gpuName || maxLen == 0) return false;

    for (size_t i=0;i<sizeof(PCI_TABLE)/sizeof(PCI_TABLE[0]);++i){
        if (PCI_TABLE[i].vendor == vendorId && PCI_TABLE[i].device == deviceId){
            String_SafeCopy(gpuName, maxLen, PCI_TABLE[i].name);
            return true;
        }
    }

    // Fallback lisible
    snprintf(gpuName, maxLen, "Graphics Device (0x%04X:0x%04X)", vendorId, deviceId);
    return false;
}
