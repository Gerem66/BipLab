#ifndef CHECKPOINT_H
#define CHECKPOINT_H

#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#ifdef _WIN32
    #include <direct.h>  // For _mkdir on Windows
#endif

// Forward declaration to avoid circular inclusion
typedef struct Map Map;

// Checkpoint settings
#define CHECKPOINT_SAVE_INTERVAL 100    // Save every X generations
#define CHECKPOINT_MAX_FILES 20         // Keep the N last checkpoints
#define CHECKPOINT_DIR "checkpoints/"   // Directory to store checkpoints

// Checkpoint functions
void Checkpoint_createDir(void);
void Checkpoint_cleanupOld(void);
void Checkpoint_save(Map *map);

#endif // CHECKPOINT_H
