#include "../../../include/core/game.h"

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
            case SDLK_v:
                map->verticalSync = !map->verticalSync;
                break;
            case SDLK_p:
                map->isRunning = !map->isRunning;
                break;
            case SDLK_c:
                Checkpoint_save(map);
                map->lastCheckpointGeneration = map->generation;
                break;
            case SDLK_g:
                // Check if Shift is pressed
                if (SDL_GetModState() & KMOD_SHIFT) {
                    // Maj+G: Open/close graph in separate window
                    if (map->graphWindowOpen) {
                        GraphWindow_Destroy(map);
                    } else {
                        GraphWindow_Create(map);
                    }
                } else {
                    // G: Toggle graph in main window
                    map->renderScoreGraph = !map->renderScoreGraph;
                }
                break;
            default:
                break;
        }
    }

    // MARK: Manual control for player cell

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

    // MARK: Zoom and drag with mouse

    if (event->type == SDL_MOUSEWHEEL) {
        SDL_GetMouseState(&mouseX, &mouseY);

        // Store old zoom for smooth transition
        float oldZoom = map->zoomFactor;

        // Scroll up
        if (event->wheel.y > 0) {
            map->zoomFactor = CLAMP(map->zoomFactor + CONTROLS_ZOOM_SPEED, CONTROLS_ZOOM_MIN, CONTROLS_ZOOM_MAX);
        }

        // Scroll down
        else if (event->wheel.y < 0) {
            map->zoomFactor = CLAMP(map->zoomFactor - CONTROLS_ZOOM_SPEED, CONTROLS_ZOOM_MIN, CONTROLS_ZOOM_MAX);
        }

        // Center view on mouse position when zooming
        if (map->zoomFactor != oldZoom) {
            // Convert mouse position to world coordinates before zoom
            float worldMouseX = (mouseX / oldZoom) + map->viewOffset.x;
            float worldMouseY = (mouseY / oldZoom) + map->viewOffset.y;

            // Calculate new view offset to keep mouse position centered
            map->viewOffset.x = worldMouseX - (mouseX / map->zoomFactor);
            map->viewOffset.y = worldMouseY - (mouseY / map->zoomFactor);

            // Clamp to valid bounds to prevent overshooting
            float maxOffsetX = MAX(0, map->width - (map->width / map->zoomFactor));
            float maxOffsetY = MAX(0, map->height - (map->height / map->zoomFactor));

            map->viewOffset.x = CLAMP(map->viewOffset.x, 0, maxOffsetX);
            map->viewOffset.y = CLAMP(map->viewOffset.y, 0, maxOffsetY);
        }

        // Update cursor based on zoom level
        if (map->zoomFactor > 1.0f && !map->isDragging) {
            SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
        } else if (map->zoomFactor <= 1.0f) {
            SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
        }
    }

    // Drag and drop for view panning
    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
        if (map->zoomFactor > 1.0f) {
            map->isDragging = true;
            SDL_GetMouseState(&map->dragStartMouse.x, &map->dragStartMouse.y);
            map->dragStartView = map->viewOffset;

            // Change cursor to closed hand
            SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
        }
    }

    if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT) {
        if (map->isDragging) {
            map->isDragging = false;

            // Restore default cursor based on context
            if (map->zoomFactor > 1.0f) {
                SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
            } else {
                SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
            }
        }
    }

    if (event->type == SDL_MOUSEMOTION && map->isDragging) {
        SDL_GetMouseState(&mouseX, &mouseY);

        // Calculate mouse displacement
        int deltaX = mouseX - map->dragStartMouse.x;
        int deltaY = mouseY - map->dragStartMouse.y;

        // Apply inverted displacement to view (inverse panning)
        map->viewOffset.x = map->dragStartView.x - (deltaX / map->zoomFactor);
        map->viewOffset.y = map->dragStartView.y - (deltaY / map->zoomFactor);

        // Clamp to valid bounds
        float maxOffsetX = MAX(0, map->width - (map->width / map->zoomFactor));
        float maxOffsetY = MAX(0, map->height - (map->height / map->zoomFactor));

        map->viewOffset.x = CLAMP(map->viewOffset.x, 0, maxOffsetX);
        map->viewOffset.y = CLAMP(map->viewOffset.y, 0, maxOffsetY);
    }
}
