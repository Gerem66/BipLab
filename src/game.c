#include "game.h"

bool Game_start(SDL_Renderer *renderer, int w, int h)
{
    Map map;
    map.width = w;
    map.height = h;
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

        map.cells[i] = Cell_init(sprite, map.width / 2, map.height / 2, i > 0);
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

    // Initialize framerate manager
    const int FPS = 60;
    FPSmanager fpsmanager;
    SDL_initFramerate(&fpsmanager);
    SDL_setFramerate(&fpsmanager, FPS);

    // Event loop exit flag
    bool quit = false;

    // View offset
    float zoomFactor = 1.0f;
    float zoomSpeed = 0.2f;
    int viewOffsetX = 0;
    int viewOffsetY = 0;
    int mouseX = 0;
    int mouseY = 0;

    // Event loop
    while (!quit)
    {
        SDL_Event e;

        // Get available event
        while (SDL_PollEvent(&e))
        {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = true;
                break;
            }

            if (e.type == SDL_KEYDOWN)
            {
                switch(e.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    case SDLK_z:
                        map.cells[0]->goingUp = true;
                        break;
                    case SDLK_q:
                        map.cells[0]->goingLeft = true;
                        break;
                    case SDLK_s:
                        map.cells[0]->goingDown = true;
                        break;
                    case SDLK_d:
                        map.cells[0]->goingRight = true;
                        break;
                    case SDLK_n:
                        map.renderNeuralNetwork = !map.renderNeuralNetwork;
                        break;
                    case SDLK_r:
                        //Game_reset(&map);
                        break;
                    case SDLK_t:
                        map.renderText = !map.renderText;
                        break;
                    case SDLK_y:
                        map.renderRays = !map.renderRays;
                        break;
                    case SDLK_i:
                        map.renderEnabled = !map.renderEnabled;
                        break;
                    case SDLK_o:
                        map.verticalSync = !map.verticalSync;
                        break;
                    case SDLK_p:
                        map.isRunning = !map.isRunning;
                        break;
                    default:
                        break;
                }
            }

            else if (e.type == SDL_KEYUP)
            {
                switch(e.key.keysym.sym)
                {
                    case SDLK_z:
                        map.cells[0]->goingUp = false;
                        break;
                    case SDLK_q:
                        map.cells[0]->goingLeft = false;
                        break;
                    case SDLK_s:
                        map.cells[0]->goingDown = false;
                        break;
                    case SDLK_d:
                        map.cells[0]->goingRight = false;
                        break;
                    default:
                        break;
                }
            }

            if (e.type == SDL_MOUSEWHEEL) {
                SDL_GetMouseState(&mouseX, &mouseY);

                if (e.wheel.y > 0) { // scroll up
                    zoomFactor += zoomSpeed;
                } else if (e.wheel.y < 0) { // scroll down
                    zoomFactor -= zoomSpeed;
                }
                zoomFactor = MAX(1.0f, MIN(zoomFactor, 10.0f));

                // Ajuster les décalages en fonction de la position de la souris
                viewOffsetX = mouseX - (map.width / zoomFactor) / 2;
                viewOffsetY = mouseY - (map.height / zoomFactor) / 2;

                // Limiter les décalages pour ne pas dépasser les limites de la carte
                viewOffsetX = MAX(0, MIN(viewOffsetX, map.width - (map.width / zoomFactor)));
                viewOffsetY = MAX(0, MIN(viewOffsetY, map.height - (map.height / zoomFactor)));
            } else if (e.type == SDL_MOUSEMOTION && zoomFactor >= 1.0f) {
                SDL_GetMouseState(&mouseX, &mouseY);
                viewOffsetX = mouseX - (map.width / zoomFactor) / 2;
                viewOffsetY = mouseY - (map.height / zoomFactor) / 2;
                viewOffsetX = MAX(0, MIN(viewOffsetX, map.width - (map.width / zoomFactor)));
                viewOffsetY = MAX(0, MIN(viewOffsetY, map.height - (map.height / zoomFactor)));
            }
        }

        // Update
        if (map.isRunning)
        {
            for (int i = 0; i < map.cellCount; ++i)
                if (map.cells[i] != NULL)
                    Cell_update(map.cells[i], &map);
            map.frames++;
        }

        // Render
        Utils_setBackgroundColor(renderer, COLOR_DARK_GRAY);
        if (map.renderEnabled)
            Game_render(&map);

        // Render neural network
        if (map.renderNeuralNetwork)
        {
            int index = map.currentBestCellIndex;
            if (map.cells[index] != NULL)
                NeuralNetwork_Render(map.cells[index], renderer, index, 900, 400, 300, 400);
        }

        // Check generation
        bool allDead = true;
        for (int i = 1; i < map.cellCount; ++i)
        {
            if (map.cells[i] != NULL && map.cells[i]->isAlive)
            {
                allDead = false;
                break;
            }
        }

        if (allDead)
        {
            Game_reset(&map);
        }

        int bestCellIndex = 1;
        for (int i = 2; i < map.cellCount; ++i)
        {
            if (map.cells[i] == NULL || map.cells[bestCellIndex] == NULL)
                continue;
            if (map.cells[i]->score > map.cells[bestCellIndex]->score)
                bestCellIndex = i;
        }
        map.currentBestCellIndex = bestCellIndex;

        // Show messages
        if (map.renderText)
            Render_Text(&map, COLOR_LIGHT_GRAY);

        // Zoom
        SDL_RenderSetScale(renderer, zoomFactor, zoomFactor);
        SDL_RenderSetViewport(renderer, &(SDL_Rect) {
            -viewOffsetX,
            -viewOffsetY,
            map.width,
            map.height
        });

        // Update screen
        SDL_RenderPresent(renderer);

        // Delay
        if (map.verticalSync)
            SDL_framerateDelay(&fpsmanager);
    }

    return true;
}

void Game_reset(Map *map)
{
    // Get best cell
    Cell *bestCell = map->cells[1];
    for (int i = 2; i < map->cellCount; ++i)
        if (map->cells[i] != NULL && map->cells[i]->score > bestCell->score)
            bestCell = map->cells[i];

    if (bestCell->score > map->maxScore)
        map->maxScore = bestCell->score;

    // Reset & mutate firsts cells state
    int revived = 0;
    while (revived < GAME_START_CELL_COUNT)
    {
        int bestIndex = -1;
        int bestScore = -1;
        for (int i = 0; i < map->cellCount; ++i)
        {
            if (map->cells[i] == NULL)
                continue;

            if (!map->cells[i]->isAlive && map->cells[i]->score > bestScore)
            {
                bestIndex = i;
                bestScore = map->cells[i]->score;
            }
        }
        if (bestIndex == -1)
        {
            fprintf(stderr, "Error while reviving cells !\n");
            return;
        }
        Cell_reset(map->cells[bestIndex]);
        Cell_mutate(map->cells[bestIndex], bestCell, 1.0f, 0.25f);
        revived++;
    }

    // Reset foods state
    for (int i = 0; i < GAME_START_FOOD_COUNT; ++i)
        Food_reset(map->foods[i], map);

    // Reset walls state
    for (int i = 0; i < WALL_COUNT; ++i)
        Wall_reset(map->walls[i], map);

    map->frames = 1;
    map->generation++;
}

void Game_render(Map *map)
{
    // Render foods
    for (int i = 0; i < GAME_START_FOOD_COUNT; ++i)
        Food_render(map->foods[i], map->renderer, map->renderText);

    // Render cells
    for (int i = 0; i < map->cellCount; ++i)
        if (map->cells[i] != NULL)
            Cell_render(map->cells[i], map->renderer, map->renderRays, i == map->currentBestCellIndex);

    // Render walls
    for (int i = 0; i < WALL_COUNT; ++i)
        Wall_render(map->walls[i], map->renderer);
}

void Render_Text(Map *map, SDL_Color color)
{
    char message[100];

    // Game informations
    time_t currentTime = time(NULL) - map->startTime;
    sprintf(message, "Time: %dm %ds (Frame %d)", (int)(currentTime / 60), (int)(currentTime % 60), map->frames % 1000);
    stringRGBA(map->renderer, 100, 25, message, color.r, color.g, color.b, color.a);

    Cell *oldestCell = map->cells[1];
    for (int i = 1; i < map->cellCount; ++i)
        if (map->cells[i] != NULL && map->cells[i]->generation > oldestCell->generation)
            oldestCell = map->cells[i];
    if (oldestCell->generation > map->maxGeneration)
        map->maxGeneration = oldestCell->generation;
    sprintf(message, "Generation: %d (max cell gen: %d)", map->generation, map->maxGeneration);
    stringRGBA(map->renderer, 100, 50, message, color.r, color.g, color.b, color.a);

    int aliveCount = 0;
    for (int i = 0; i < map->cellCount; ++i)
        if (map->cells[i] != NULL && map->cells[i]->isAlive)
            aliveCount++;
    sprintf(message, "Cells count: %d (total: %d)", aliveCount, map->cellCount - 1);
    stringRGBA(map->renderer, 100, 75, message, color.r, color.g, color.b, color.a);

    Cell *bestCell = map->cells[1];
    for (int i = 2; i < map->cellCount; ++i)
        if (map->cells[i] != NULL && map->cells[i]->score > bestCell->score)
            bestCell = map->cells[i];
    sprintf(message, "Best score: %d (max: %d)", bestCell->score, map->maxScore);
    stringRGBA(map->renderer, 100, 100, message, color.r, color.g, color.b, color.a);

    // Player informations
    sprintf(message, "Player pos: %d, %d", (int)map->cells[0]->position.x, (int)map->cells[0]->position.y);
    stringRGBA(map->renderer, 500, 25, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Angle: %f", map->cells[0]->angle);
    stringRGBA(map->renderer, 500, 75, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Speed: %f", map->cells[0]->speed);
    stringRGBA(map->renderer, 500, 50, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Score: %d", map->cells[0]->score);
    stringRGBA(map->renderer, 500, 100, message, color.r, color.g, color.b, color.a);

    // Render controls
    sprintf(message, "N: Show/Hide neural network");
    stringRGBA(map->renderer, 850, 25, message, color.r, color.g, color.b, color.a);

    sprintf(message, "T: Show/Hide texts");
    stringRGBA(map->renderer, 850, 50, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Y: Enable/Disable render of rays");
    stringRGBA(map->renderer, 850, 75, message, color.r, color.g, color.b, color.a);

    sprintf(message, "I: Enable/Disable render of cells");
    stringRGBA(map->renderer, 850, 100, message, color.r, color.g, color.b, color.a);

    sprintf(message, "O: Enable/Disable vertical sync");
    stringRGBA(map->renderer, 850, 125, message, color.r, color.g, color.b, color.a);

    sprintf(message, "P: Pause/Unpause cells evolution");
    stringRGBA(map->renderer, 850, 150, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Esc: Quit");
    stringRGBA(map->renderer, 850, 175, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Mouse wheel: Zoom/Dezoom & move view");
    stringRGBA(map->renderer, 850, 200, message, color.r, color.g, color.b, color.a);
}
