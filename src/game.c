#include "game.h"

bool Game_start(SDL_Renderer *renderer, int w, int h)
{
    (void) w;
    (void) h;

    Map map;
    map.startTime = time(NULL);
    map.generation = 1;
    map.frames = 1;
    map.maxScore = 0;
    map.maxAverageScore = 0;
    map.isRunning = true;
    map.verticalSync = true;
    map.renderEnabled = true;

    // Initialize walls
    map.walls[0] = Wall_init(0.0f, 200.0f, 750.0f, 10.0f);
    map.walls[1] = Wall_init(0.0f, 400.0f, 750.0f, 10.0f);
    map.walls[2] = Wall_init(0.0f, 200.0f, 10.0f, 200.0f);
    map.walls[3] = Wall_init(750.0f, 200.0f, 10.0f, 210.0f);
    if (map.walls[0] == NULL || map.walls[1] == NULL)
    {
        fprintf(stderr, "Error while initializing walls !\n");
        return false;
    }

    // Initialize cells
    for(int i = 0; i < CELL_COUNT; ++i)
    {
        map.cells[i] = Cell_init(50, 300, i > 0);
        if (map.cells[i] == NULL)
        {
            fprintf(stderr, "Error while initializing cell %d !\n", i);
            for(int j = 0; j < i; ++j)
            {
                Cell_destroy(map.cells[j]);
            }
            return false;
        }
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
                        Game_reset(&map);
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
            for (int i = 0; i < CELL_COUNT; ++i)
                Cell_update(map.cells[i], &map);
            map.frames++;
        }

        // Render
        Utils_setBackgroundColor(renderer, COLOR_DARK_GRAY);
        if (map.renderEnabled)
            Game_render(renderer, &map);

        // Check generation
        bool allDead = true;
        for (int i = 1; i < CELL_COUNT; ++i)
        {
            if (map.cells[i]->isAlive)
            {
                allDead = false;
                break;
            }
        }

        if (allDead || map.frames % 800 == 0)
        {
            Game_reset(&map);
        }

        // Show messages
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
    // Get max average score
    float averageScore = 0.0f;
    for (int i = 1; i < CELL_COUNT; ++i)
    {
        averageScore += map->cells[i]->score;
        if (map->cells[i]->score > map->maxScore)
            map->maxScore = map->cells[i]->score;
    }
    averageScore /= (CELL_COUNT - 1);
    if (averageScore > map->maxAverageScore)
        map->maxAverageScore = averageScore;

    // Get best cell
    Cell *bestCell = map->cells[1];
    for (int i = 2; i < CELL_COUNT; ++i)
        if (map->cells[i]->score > bestCell->score)
            bestCell = map->cells[i];

    // Mutate cells
    for (int i = 1; i < CELL_COUNT; ++i)
        Cell_mutate(map->cells[i], bestCell, 0.2f, 0.05f);

    // Reset cells state
    for (int i = 0; i < CELL_COUNT; ++i)
        Cell_reset(map->cells[i]);

    map->frames = 1;
    map->generation++;
}

void Game_render(SDL_Renderer *renderer, Map *map)
{
        // Render walls
        for (int i = 0; i < 4; ++i)
            Wall_render(map->walls[i], renderer);

        // Render cells
        for (int i = 0; i < CELL_COUNT; ++i)
            Cell_render(map->cells[i], renderer);
}

void Render_Text(SDL_Renderer *renderer, Map *map, SDL_Color color)
{
    char message[100];

    time_t currentTime = time(NULL) - map->startTime;
    sprintf(message, "Time: %dm %ds", (int)(currentTime / 60), (int)(currentTime % 60));
    stringRGBA(renderer, 100, 25, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Generation: %d (Frame %d/1000)", map->generation, map->frames % 1000);
    stringRGBA(renderer, 100, 50, message, color.r, color.g, color.b, color.a);

    int aliveCount = 0;
    for (int i = 0; i < CELL_COUNT; ++i)
        if (map->cells[i]->isAlive)
            aliveCount++;
    sprintf(message, "Cells count: %d (alive: %d)", CELL_COUNT, aliveCount);
    stringRGBA(renderer, 100, 75, message, color.r, color.g, color.b, color.a);

    Cell *bestCell = map->cells[1];
    for (int i = 2; i < CELL_COUNT; ++i)
        if (map->cells[i]->score > bestCell->score)
            bestCell = map->cells[i];
    float averageScore = 0.0f;
    for (int i = 1; i < CELL_COUNT; ++i)
        averageScore += map->cells[i]->score;
    sprintf(message, "Best score: %d (max: %d)", bestCell->score, map->maxScore);
    stringRGBA(renderer, 100, 100, message, color.r, color.g, color.b, color.a);

    sprintf(message, "Average score: %d (max: %d)", (int)(averageScore / (CELL_COUNT - 1)), map->maxAverageScore);
    stringRGBA(renderer, 100, 125, message, color.r, color.g, color.b, color.a);

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
