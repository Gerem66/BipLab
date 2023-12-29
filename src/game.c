#include "game.h"

bool Game_start(SDL_Renderer *renderer, int w, int h)
{
    Map map;
    map.width = w;
    map.height = h;
    map.startTime = time(NULL);
    map.generation = 1;
    map.frames = 1;
    map.maxScore = 0;
    map.isRunning = true;
    map.verticalSync = true;
    map.renderText = true;
    map.renderRays = false;
    map.renderEnabled = true;
    map.cellCount = 0;

    // Initialize foods
    for (int i = 0; i < FOOD_COUNT; ++i)
    {
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
        if (i > CELL_INIT)
        {
            map.cells[i] = NULL;
            continue;
        }
        map.cells[i] = Cell_init(map.width / 2, map.height / 2, i > 0);
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
            Game_render(renderer, &map);

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

        // Show messages
        if (map.renderText)
            Render_Text(renderer, &map, COLOR_LIGHT_GRAY);

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
    while (revived < CELL_INIT)
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
        Cell_mutate(map->cells[bestIndex], bestCell, 0.2f, 0.4f);
        revived++;
    }

    // Reset foods state
    for (int i = 0; i < FOOD_COUNT; ++i)
        Food_reset(map->foods[i], map);

    map->frames = 1;
    map->generation++;
}

void Game_render(SDL_Renderer *renderer, Map *map)
{
        // Render foods
        for (int i = 0; i < FOOD_COUNT; ++i)
            Food_render(map->foods[i], renderer, map->renderText);

        // Render cells
        for (int i = 0; i < map->cellCount; ++i)
            if (map->cells[i] != NULL)
                Cell_render(map->cells[i], renderer, map->renderRays);
}

void Render_Text(SDL_Renderer *renderer, Map *map, SDL_Color color)
{
    char message[100];

    time_t currentTime = time(NULL) - map->startTime;
    sprintf(message, "Time: %dm %ds (Frame %d)", (int)(currentTime / 60), (int)(currentTime % 60), map->frames % 1000);
    stringRGBA(renderer, 100, 25, message, color.r, color.g, color.b, color.a);

    Cell *oldestCell = map->cells[1];
    for (int i = 1; i < map->cellCount; ++i)
        if (map->cells[i] != NULL && map->cells[i]->generation > oldestCell->generation)
            oldestCell = map->cells[i];
    sprintf(message, "Generation: %d (max cell gen: %d)", map->generation, oldestCell->generation);
    stringRGBA(renderer, 100, 50, message, color.r, color.g, color.b, color.a);

    int aliveCount = 0;
    for (int i = 0; i < map->cellCount; ++i)
        if (map->cells[i] != NULL && map->cells[i]->isAlive)
            aliveCount++;
    sprintf(message, "Cells count: %d (total: %d)", aliveCount, map->cellCount - 1);
    stringRGBA(renderer, 100, 75, message, color.r, color.g, color.b, color.a);

    Cell *bestCell = map->cells[1];
    for (int i = 2; i < map->cellCount; ++i)
        if (map->cells[i] != NULL && map->cells[i]->score > bestCell->score)
            bestCell = map->cells[i];
    sprintf(message, "Best score: %d (max: %d)", bestCell->score, map->maxScore);
    stringRGBA(renderer, 100, 100, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Player pos: %d, %d", (int)map->cells[0]->position.x, (int)map->cells[0]->position.y);
    stringRGBA(renderer, 500, 25, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Angle: %f", map->cells[0]->angle);
    stringRGBA(renderer, 500, 75, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Speed: %f", map->cells[0]->speed);
    stringRGBA(renderer, 500, 50, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Score: %d", map->cells[0]->score);
    stringRGBA(renderer, 500, 100, message, color.r, color.g, color.b, color.a);

    // Show neural netword values of each layers
    if (bestCell->isAI)
    {
        float x = 50.0f;
        float y = 450.0f;
        for (int i = 0; i < bestCell->nn->topologySize - 1; ++i)
        {
            sprintf(message, "Layer %d:", i);
            stringRGBA(renderer, x, y + i * 25, message, color.r, color.g, color.b, color.a);

            for (int j = 0; j < bestCell->nn->layers[i]->nextLayerNeuronCount; ++j)
            {
                sprintf(message, "%f", bestCell->nn->layers[i]->weights[j]);
                stringRGBA(renderer, x + 100 * (j + 1), y + i * 25, message, color.r, color.g, color.b, color.a);
            }
        }
    }
}
