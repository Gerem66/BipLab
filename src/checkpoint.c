#include "checkpoint.h"
#include "game.h"

void Checkpoint_createDir(void)
{
    #ifdef _WIN32
        _mkdir(CHECKPOINT_DIR);
    #else
        mkdir(CHECKPOINT_DIR, 0755);
    #endif
}

void Checkpoint_cleanupOld(void)
{
    char command[256];
    #ifdef _WIN32
        snprintf(command, sizeof(command),
            "cd %s && for /f \"skip=%d\" %%i in ('dir checkpoint_*.nn /b /o-d') do del \"%%i\"",
            CHECKPOINT_DIR, CHECKPOINT_MAX_FILES);
    #else
        snprintf(command, sizeof(command),
            "cd %s && ls -t checkpoint_*.nn 2>/dev/null | tail -n +%d | xargs rm -f",
            CHECKPOINT_DIR, CHECKPOINT_MAX_FILES + 1);
    #endif

    system(command);
}

void Checkpoint_save(Map *map)
{
    if (map->bestCellEver == NULL || map->bestCellEver->nn == NULL) {
        return;
    }

    Checkpoint_createDir();

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char filename[256];
    char timestamp[64];

    strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", tm_info);
    snprintf(filename, sizeof(filename), "%scheckpoint_gen%d_%s_score%d.nn",
             CHECKPOINT_DIR, map->generation, timestamp, map->bestCellEver->score);

    if (Game_save(map, filename)) {
        map->checkpointCounter++;
        Checkpoint_cleanupOld();
    }
}
