#include "../../../include/core/game.h"
#include "../../../include/entities/cell.h"
#include "../../../include/system/embedded_resources.h"
#include "../../../include/ui/ui.h"
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>  // For getcwd()
#include <stdlib.h>  // For malloc/free
#include <string.h>   // For memset

bool Game_start(SDL_Window *window, SDL_Renderer *renderer, int w, int h)
{
    Map map;
    map.width = w;
    map.height = h;
    map.viewOffset = (SDL_Point) { 0, 0 };
    map.zoomFactor = 1.0f;
    map.isDragging = false;
    map.dragStartMouse = (SDL_Point) { 0, 0 };
    map.dragStartView = (SDL_Point) { 0, 0 };

    map.startTime = time(NULL);
    map.pausedTime = 0;

    map.generation = 1;
    map.maxGeneration = 1;
    map.frames = 1;
    map.maxScore = 0;
    map.isRunning = true;
    map.verticalSync = true;
    map.renderText = true;
    map.renderRays = false;
    map.renderNeuralNetwork = false;
    map.renderScoreGraph = false;
    map.renderEnabled = true;
    map.useMultithreading = true;
    map.useGpuAcceleration = true;
    map.cellCount = 0;
    map.quit = false;

    // Initialize default values
    map.mode = SCREEN_NORMAL;
    map.currentBestCellIndex = 1;
    map.renderer = renderer;
    map.window = window;

    // Apply startup mode
    Screen_Set(&map, GAME_START_MODE);

    // Initialize checkpoint variables
    map.lastCheckpointGeneration = 0;
    map.checkpointCounter = 0;

    // Initialize performance tracking
    map.previousGenFrames = 0;
    map.currentFPS = 0;
    map.currentUPS = 0;
    map.currentGPS = 0.0f;

    // Initialize graph system
    if (!Graph_Init(&map.graphData)) {
        fprintf(stderr, "Failed to initialize graph system!\n");
        return false;
    }

    // Initialize evolution system
    Evolution_InitMutationParams(&map.mutationParams);
    memset(&map.evolutionMetrics, 0, sizeof(EvolutionMetrics));

    // Initialize graph window
    map.graphWindow = NULL;
    map.graphRenderer = NULL;
    map.graphWindowOpen = false;

    // Initialize walls
    for (int i = 0; i < GAME_START_WALL_COUNT; ++i)
    {
        map.walls[i] = Wall_init(0, 0, 40, 40);
        if (map.walls[i] == NULL)
        {
            fprintf(stderr, "Error while initializing wall %d !\n", i);
            for (int j = 0; j < i; ++j)
            {
                Wall_destroy(map.walls[j]);
            }
            return false;
        }
        Wall_reset(map.walls[i], &map);
    }

    // Initialize foods
    for (int i = 0; i < MEM_FOOD_COUNT; ++i)
    {
        if (i > GAME_START_FOOD_COUNT)
        {
            map.foods[i] = NULL;
            continue;
        }
        map.foods[i] = Food_init(irand(0, map.width), irand(0, map.height));
        if (map.foods[i] == NULL)
        {
            fprintf(stderr, "Error while initializing food %d !\n", i);
            for (int j = 0; j < i; ++j)
            {
                Food_destroy(map.foods[j]);
            }
            return false;
        }
    }

    // Load textures once if sprite rendering is enabled (optimized loading)
    SDL_Texture *normalSprite = NULL;
    SDL_Texture *shinySprite = NULL;

    if (CELL_USE_SPRITE) {
        EmbeddedResource normalSkin = get_embedded_resource(RES_SKIN_NORMAL);
        normalSprite = load_texture_from_embedded_data(renderer, normalSkin.data, normalSkin.size);

        EmbeddedResource shinySkin = get_embedded_resource(RES_SKIN_SHINY);
        shinySprite = load_texture_from_embedded_data(renderer, shinySkin.data, shinySkin.size);

        if (!normalSprite || !shinySprite) {
            printf("Failed to load sprites from embedded data: %s\n", SDL_GetError());
            return false;
        }

        if (!load_all_cell_sprites(renderer)) {
            fprintf(stderr, "Failed to load cell sprites!\n");
            return false;
        }
    }

    // Initialize cells
    if (GAME_START_CELL_COUNT <= 0 || GAME_START_CELL_COUNT > MEM_CELL_COUNT)
    {
        fprintf(stderr, "Error while initializing cells !\n");
        return false;
    }
    for (int i = 0; i < MEM_CELL_COUNT; ++i)
    {
        if (i >= GAME_START_CELL_COUNT)
        {
            map.cells[i] = NULL;
            continue;
        }

        // Create cell with shared sprite texture (no individual loading)
        map.cells[i] = Cell_create(map.width / 2, map.height / 2, !CELL_AS_PLAYER || i > 0);
        if (map.cells[i] == NULL)
        {
            fprintf(stderr, "Error while initializing cell %d !\n", i);
            for (int j = 0; j < i; ++j)
                if (map.cells[j] != NULL)
                    Cell_destroy(map.cells[j]);
            return false;
        }
        map.cellCount++;
    }

    // Initialize best cell ever with shiny sprite
    map.bestCellEver = Cell_create(map.width / 2, map.height / 2, false);

    // Load a neural network if file exists
    char filename[] = "../ressources/best.nn";

    int popup_result = 0;

    if (Game_exists(filename))
    {
        popup_result = open_popup_ask(
            "Neural network found !",
            "Do you want to load it ?"
        );
    }

    NeuralNetwork *nn = NULL;
    if (popup_result == 1)
    {
        nn = Game_load(&map, filename);
        for (int i = 0; i < map.cellCount; ++i)
        {
            if (map.cells[i] != NULL)
            {
                NeuralNetwork *newNN = NeuralNetwork_Copy(nn);
                if (newNN == NULL)
                {
                    fprintf(stderr, "Failed to copy NeuralNetwork !\n");
                    return false;
                }

                freeNeuralNetwork(map.cells[i]->nn);
                map.cells[i]->nn = newNN;
            }
        }
        printf("Neural network loaded !\n");
    }
    if (nn != NULL)
    {
        freeNeuralNetwork(nn);
        nn = NULL;
    }

    // Initialize framerate manager
    FPSmanager fpsmanager;
    SDL_initFramerate(&fpsmanager);
    if (GAME_FPS_LIMIT > 0) SDL_setFramerate(&fpsmanager, GAME_FPS_LIMIT);

    // Initialize render timing variables
    Uint32 lastRenderTime = SDL_GetTicks();
    Uint32 renderIntervalMs = (GAME_FPS_LIMIT > 0) ? (1000 / GAME_FPS_LIMIT) : 0;

    // FPS tracking
    static time_t lastFPSTime = 0;
    static int renderFrameCount = 0;

    // Event loop
    while (!map.quit)
    {
        // Handle events
        SDL_Event e;
        while (SDL_PollEvent(&e))
            Game_events(&map, &e);

        // Update
        Game_update(&map);

        // Track paused time
        static time_t lastPauseStart = 0;
        if (!map.isRunning && lastPauseStart == 0) { // Pause started
            lastPauseStart = time(NULL);
        } else if (map.isRunning && lastPauseStart != 0) { // Pause ended
            map.pausedTime += time(NULL) - lastPauseStart;
            lastPauseStart = 0;
        }

        // Render only if enough time has passed since last render (for FPS control)
        Uint32 currentTime = SDL_GetTicks();
        bool shouldRender = true;
        if (GAME_FPS_LIMIT > 0 && renderIntervalMs > 0) {
            shouldRender = (currentTime - lastRenderTime) >= renderIntervalMs;
        }

        // Render if needed
        if (shouldRender) {
            GameInterface_Render(renderer, &map);

            // Render graph window if open
            GraphWindow_Render(&map);

            lastRenderTime = currentTime;

            // FPS tracking
            renderFrameCount++;

            time_t currentFPSTime = time(NULL);
            if (currentFPSTime != lastFPSTime && lastFPSTime != 0) {
                map.currentFPS = renderFrameCount;
                renderFrameCount = 0;
            }
            lastFPSTime = currentFPSTime;
        }

        // Delay to cap framerate if vertical sync is enabled and FPS limit is set
        if (map.verticalSync && GAME_FPS_LIMIT > 0)
            SDL_framerateDelay(&fpsmanager);
    }

    // Free loaded textures
    if (CELL_USE_SPRITE) {
        if (normalSprite) {
            SDL_DestroyTexture(normalSprite);
        }
        if (shinySprite) {
            SDL_DestroyTexture(shinySprite);
        }
    }

    int popup_save_result = open_popup_ask(
        "Quit without saving ?",
        "Do you want to save the neural network ?"
    );
    if (popup_save_result == 1)
    {
        bool saved = Game_save(&map, "../ressources/best.nn");
        if (!saved)
            open_popup_message(
                "Error while saving !",
                "An error occured while saving the neural network !"
            );
    }

    // Clean up graph window
    GraphWindow_Destroy(&map);

    // Free graph system
    Graph_Free(&map.graphData);

    return true;
}
