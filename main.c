#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "core/game.h"
#include "core/config.h"
#include "entities/cell.h"

int main(int argc, char* argv[])
{
    // Unused argc, argv
    (void) argc;
    (void) argv;

    // Verify neural network topology consistency
    int topology[] = NEURAL_NETWORK_TOPOLOGY;
    int expected_inputs = CELL_PERCEPTION_RAYS * RAY_OBJECT_COUNT + 2; // rays * types + health + can_reproduce

    if (topology[0] != expected_inputs) {
        fprintf(stderr, "ERROR: Neural network topology mismatch!\n");
        fprintf(stderr, "Expected: %d * %d + 2 = %d inputs, but got: %d inputs\n",
                CELL_PERCEPTION_RAYS, RAY_OBJECT_COUNT, expected_inputs, topology[0]);
        return 1;
    }

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL could not be initialized!\n"
                        "SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

#if defined linux && SDL_VERSION_ATLEAST(2, 0, 8)
    // Disable compositor bypass
    if (!SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0"))
    {
        printf("SDL can not disable compositor bypass!\n");
        return 0;
    }
#endif

    // Create window
    SDL_Window *window = SDL_CreateWindow("Cells Evolution - SDL",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        fprintf(stderr, "Window could not be created!\n"
                        "SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
        // Create renderer
        SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer)
        {
            fprintf(stderr, "Renderer could not be created!\n"
                            "SDL_Error: %s\n", SDL_GetError());
        }
        else
        {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

            // Start the game
            Game_start(window, renderer, GAME_WIDTH, GAME_HEIGHT);

            // Destroy renderer
            SDL_DestroyRenderer(renderer);
        }

        // Destroy window
        SDL_DestroyWindow(window);
    }

    // Quit SDL
    SDL_Quit();

    return 0;
}
