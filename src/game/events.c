#include "game.h"

void Game_events(Map *map, SDL_Event *event)
{
    int mouseX, mouseY;

    // User requests quit
    if (event->type == SDL_QUIT) {
        map->quit = true;
        return;
    }

    if (event->type == SDL_KEYDOWN)
    {
        switch(event->key.keysym.sym)
        {
            case SDLK_ESCAPE:
                map->quit = true;
                break;
#if CELL_AS_PLAYER
            case SDLK_z:
                map->cells[0]->goingUp = true;
                break;
            case SDLK_q:
                map->cells[0]->goingLeft = true;
                break;
            case SDLK_s:
                map->cells[0]->goingDown = true;
                break;
            case SDLK_d:
                map->cells[0]->goingRight = true;
                break;
#endif
            case SDLK_n:
                map->renderNeuralNetwork = !map->renderNeuralNetwork;
                break;
            case SDLK_r:
                Game_reset(map, true);
                break;
            case SDLK_RETURN:
                if (Game_save(map, "../ressources/best.nn"))
                    open_popup_message(
                        "Neural network saved !",
                        "The neural network has been saved in the file \"best.nn\" !"
                    );
                break;
            case SDLK_t:
                map->renderText = !map->renderText;
                break;
            case SDLK_y:
                map->renderRays = !map->renderRays;
                break;
            case SDLK_i:
                map->renderEnabled = !map->renderEnabled;
                break;
            case SDLK_o:
                map->verticalSync = !map->verticalSync;
                break;
            case SDLK_p:
                map->isRunning = !map->isRunning;
                break;
            case SDLK_c:
                Checkpoint_save(map);
                map->lastCheckpointGeneration = map->generation;
                break;
            default:
                break;
        }
    }

    else if (event->type == SDL_KEYUP)
    {
#if CELL_AS_PLAYER
        switch(event->key.keysym.sym)
        {
            case SDLK_z:
                map->cells[0]->goingUp = false;
                break;
            case SDLK_q:
                map->cells[0]->goingLeft = false;
                break;
            case SDLK_s:
                map->cells[0]->goingDown = false;
                break;
            case SDLK_d:
                map->cells[0]->goingRight = false;
                break;
            default:
                break;
        }
#endif
    }

    if (event->type == SDL_MOUSEWHEEL) {
        // Scroll up
        if (event->wheel.y > 0) {
            map->zoomFactor = CLAMP(map->zoomFactor + CONTROLS_ZOOM_SPEED, 0.5f, 10.0f);
        }

        // Scroll down
        else if (event->wheel.y < 0) {
            map->zoomFactor = CLAMP(map->zoomFactor - CONTROLS_ZOOM_SPEED, 0.5f, 10.0f);
        }

        // Move view to mouse position
        SDL_GetMouseState(&mouseX, &mouseY);
        float maximumOffsetX = map->width - (map->width / map->zoomFactor);
        float maximumOffsetY = map->height - (map->height / map->zoomFactor);
        map->viewOffset.x = CLAMP(mouseX - (map->width / map->zoomFactor) / 2, 0, maximumOffsetX);
        map->viewOffset.y = CLAMP(mouseY - (map->height / map->zoomFactor) / 2, 0, maximumOffsetY);
    } else if (event->type == SDL_MOUSEMOTION && map->zoomFactor >= 1.0f) {
        SDL_GetMouseState(&mouseX, &mouseY);
        float maximumOffsetX = map->width - (map->width / map->zoomFactor);
        float maximumOffsetY = map->height - (map->height / map->zoomFactor);
        map->viewOffset.x = CLAMP(mouseX - (map->width / map->zoomFactor) / 2, 0, maximumOffsetX);
        map->viewOffset.y = CLAMP(mouseY - (map->height / map->zoomFactor) / 2, 0, maximumOffsetY);
    }
}
