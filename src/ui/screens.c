#include "../../include/ui/ui.h"

static struct { int w, h; bool vsync; } SCREENS[] = {
    [SCREEN_NORMAL] = {SCREEN_WIDTH, SCREEN_HEIGHT, true},
    [SCREEN_TRAINING] = {TRAINING_SCREEN_WIDTH, TRAINING_SCREEN_HEIGHT, false},
    [SCREEN_DEBUG] = {1600, 900, false},
    [SCREEN_FULLSCREEN] = {1920, 1080, true}
};

void Screen_Set(Map *map, ScreenMode mode)
{
    SCREENS[map->mode].vsync = map->verticalSync;  // Save current
    map->verticalSync = SCREENS[mode].vsync;       // Apply new
    map->mode = mode;
    Game_ResizeWindow(map, SCREENS[mode].w, SCREENS[mode].h);
}
