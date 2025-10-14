#ifndef UI_H
#define UI_H

#include "../core/game.h"

// Screen management
typedef enum {
    SCREEN_NORMAL,
    SCREEN_TRAINING,
    SCREEN_DEBUG,
    SCREEN_FULLSCREEN
} ScreenMode;

void Screen_Set(Map *map, ScreenMode mode);

#endif // UI_H
