#include "game.h"

bool Game_start(SDL_Renderer *renderer, int w, int h)
{
    Map map;
    map.width = w;
    map.height = h;
    map.viewOffset = (SDL_Point) { 0, 0 };
    map.zoomFactor = 1.0f;

    map.startTime = time(NULL);
    map.generation = 1;
    map.maxGeneration = 1;
    map.frames = 1;
    map.maxScore = 0;
    map.isRunning = true;
    map.verticalSync = true;
    map.renderText = true;
    map.renderRays = false;
    map.renderNeuralNetwork = false;
    map.renderEnabled = true;
    map.cellCount = 0;
    map.quit = false;
    map.currentBestCellIndex = 1;
    map.renderer = renderer;

    // Initialize walls
    for (int i = 0; i < WALL_COUNT; ++i)
    {
        map.walls[i] = Wall_init(0, 0, 40, 40);
        if (map.walls[i] == NULL)
        {
            fprintf(stderr, "Error while initializing wall %d !\n", i);
            for(int j = 0; j < i; ++j)
            {
                Wall_destroy(map.walls[j]);
            }
            return false;
        }
        Wall_reset(map.walls[i], &map);
    }

    // Initialize foods
    for (int i = 0; i < FOOD_COUNT; ++i)
    {
        if (i > GAME_START_FOOD_COUNT)
        {
            map.foods[i] = NULL;
            continue;
        }
        map.foods[i] = Food_init(Utils_rand(0, map.width), Utils_rand(0, map.height));
        if (map.foods[i] == NULL)
        {
            fprintf(stderr, "Error while initializing food %d !\n", i);
            for(int j = 0; j < i; ++j)
            {
                Food_destroy(map.foods[j]);
            }
            return false;
        }
    }

    // Initialize cells
    for(int i = 0; i < CELL_COUNT; ++i)
    {
        if (i > GAME_START_CELL_COUNT)
        {
            map.cells[i] = NULL;
            continue;
        }

        // Load cell sprite
        SDL_Texture *sprite = LoadSprite(renderer, "../ressources/cell.png");
        if (sprite == NULL)
        {
            printf("Erreur de création de la texture : %s", SDL_GetError());
            continue;
        }

        map.cells[i] = Cell_create(sprite, map.width / 2, map.height / 2, i > 0);
        if (map.cells[i] == NULL)
        {
            fprintf(stderr, "Error while initializing cell %d !\n", i);
            for(int j = 0; j < i; ++j)
                if (map.cells[j] != NULL)
                    Cell_destroy(map.cells[j]);
            return false;
        }
        map.cellCount++;
    }

    // Load a neural network if file exists
    NeuralNetwork *nn = loadNeuralNetwork("../ressources/best.nn");
    if (nn != NULL)
    {
        int popup_result = open_popup_ask(
            "Neural network found !",
            "Do you want to load it ?"
        );
        if (popup_result == 1)
        {
            for (int i = 0; i < map.cellCount; ++i)
                if (map.cells[i] != NULL)
                    mutateNeuralNetwork(map.cells[i]->nn, nn, 1.0f, 0.0f);
            printf("Neural network loaded !\n");
        }
        freeNeuralNetwork(nn);
    }

    // Initialize framerate manager
    const int FPS = 60;
    FPSmanager fpsmanager;
    SDL_initFramerate(&fpsmanager);
    SDL_setFramerate(&fpsmanager, FPS);

    // Event loop
    while (!map.quit)
    {
        // Handle events
        SDL_Event e;
        while (SDL_PollEvent(&e))
            Game_events(&map, &e);

        // Update
        Game_update(&map);

        // Render
        Game_render(renderer, &map);

        // Delay
        if (map.verticalSync)
            SDL_framerateDelay(&fpsmanager);
    }

    return true;
}
