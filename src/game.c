#include "game.h"

bool Game_start(SDL_Renderer *renderer, int w, int h)
{
    (void) w;
    (void) h;

    // Init grid
    Cell *cells[10];

    for(int i = 0; i < 10; ++i)
    {
        cells[i] = Cell_init(0, 0, 10);
        if (cells[i] == NULL)
        {
            fprintf(stderr, "Error while initializing cell %d !\n", i);
            for(int j = 0; j < i; ++j)
            {
                Cell_destroy(cells[j]);
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
            if (e.type == SDL_KEYDOWN)
            {
                switch(e.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    case SDLK_z:
                        cells[0]->goingUp = true;
                        break;
                    case SDLK_q:
                        cells[0]->goingLeft = true;
                        break;
                    case SDLK_s:
                        cells[0]->goingDown = true;
                        break;
                    case SDLK_d:
                        cells[0]->goingRight = true;
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
                        cells[0]->goingUp = false;
                        break;
                    case SDLK_q:
                        cells[0]->goingLeft = false;
                        break;
                    case SDLK_s:
                        cells[0]->goingDown = false;
                        break;
                    case SDLK_d:
                        cells[0]->goingRight = false;
                        break;
                    default:
                        break;
                }
            }
        }

        // Update
        Cell_update(cells[0]);

        // Set background color
        Utils_setBackgroundColor(renderer, COLOR_DARK_GRAY);

        // Render grid
        for (int i = 0; i < 10; ++i)
        {
            Cell_render(cells[i], renderer);
        }

        // Show message
        char message[100] = "Speed: ";
        char speed[10];
        sprintf(speed, "%f", cells[0]->speed);
        strcat(message, speed);
        stringRGBA(renderer, 100, 50, message,
                   COLOR_LIGHT_GRAY.r, COLOR_LIGHT_GRAY.g, COLOR_LIGHT_GRAY.b, COLOR_LIGHT_GRAY.a);

        message[0] = '\0';
        strcat(message, "Angle: ");
        char angle_str[10];
        sprintf(angle_str, "%f", cells[0]->angle);
        strcat(message, angle_str);
        stringRGBA(renderer, 100, 75, message,
                   COLOR_LIGHT_GRAY.r, COLOR_LIGHT_GRAY.g, COLOR_LIGHT_GRAY.b, COLOR_LIGHT_GRAY.a);

        stringRGBA(renderer, 100, 100,
                   "This is a falling brick < Press RIGTH and LEFT to move >",
                   COLOR_LIGHT_GRAY.r, COLOR_LIGHT_GRAY.g, COLOR_LIGHT_GRAY.b, COLOR_LIGHT_GRAY.a);

        // Update screen
        SDL_RenderPresent(renderer);

        // Delay
        SDL_framerateDelay(&fpsmanager);
    }

    return true;
}
