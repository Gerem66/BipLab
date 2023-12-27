#include "game.h"

bool Game_start(SDL_Renderer *renderer, int w, int h)
{
    (void) w;
    (void) h;

    Map map;

    // Initialize walls
    map.walls[0] = Wall_init(0, 200, 750, 10);
    map.walls[1] = Wall_init(0, 400, 750, 10);
    map.walls[2] = Wall_init(0, 200, 10, 200);
    map.walls[3] = Wall_init(750, 200, 10, 210);
    if (map.walls[0] == NULL || map.walls[1] == NULL)
    {
        fprintf(stderr, "Error while initializing walls !\n");
        return false;
    }


    // Initialize cells
    for(int i = 0; i < 10; ++i)
    {
        map.cells[i] = Cell_init(50, 300, 10);
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
        Cell_update(map.cells[0], &map);

        // Set background color
        Utils_setBackgroundColor(renderer, COLOR_DARK_GRAY);

        // Render walls
        for (int i = 0; i < 4; ++i)
            Wall_render(map.walls[i], renderer);

        // Render cells
        for (int i = 0; i < 10; ++i)
            Cell_render(map.cells[i], renderer);

        // Show message
        char message[100];

        sprintf(message, "Speed: %f", map.cells[0]->speed);
        stringRGBA(renderer, 100, 50, message,
                   COLOR_LIGHT_GRAY.r, COLOR_LIGHT_GRAY.g, COLOR_LIGHT_GRAY.b, COLOR_LIGHT_GRAY.a);

        sprintf(message, "Angle: %f", map.cells[0]->angle);
        stringRGBA(renderer, 100, 75, message,
                   COLOR_LIGHT_GRAY.r, COLOR_LIGHT_GRAY.g, COLOR_LIGHT_GRAY.b, COLOR_LIGHT_GRAY.a);

        sprintf(message, "Score: %d", map.cells[0]->score);
        stringRGBA(renderer, 100, 100,
                   message,
                   COLOR_LIGHT_GRAY.r, COLOR_LIGHT_GRAY.g, COLOR_LIGHT_GRAY.b, COLOR_LIGHT_GRAY.a);

        // Update screen
        SDL_RenderPresent(renderer);

        // Delay
        SDL_framerateDelay(&fpsmanager);
    }

    return true;
}

void Game_reset(Map *map)
{
    for (int i = 0; i < 10; ++i)
    {
        Cell_reset(map->cells[i]);
    }
}
