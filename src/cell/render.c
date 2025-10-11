#include "../../include/cell.h"

// Global sprite structure to hold different cell part textures
typedef struct {
    SDL_Texture *skin;
    SDL_Texture *leaf;
    SDL_Texture *ass;
    SDL_Texture *eyes;
} CellSprites;

// Global sprites for normal and shiny cells
static CellSprites *normalSprites = NULL;
static CellSprites *shinySprites = NULL;

// Global flag to track sprite loading status
static bool spritesLoaded = false;

// Function prototypes to resolve implicit declaration errors
void render_healthbar(Cell *cell, SDL_Renderer *renderer);
void render_face(Cell *cell, SDL_Renderer *renderer);
void render_rays(Cell *cell, SDL_Renderer *renderer);

// Function to load cell sprites once at program start
bool load_all_cell_sprites(SDL_Renderer *renderer) {
    // Prevent reloading
    if (spritesLoaded) {
        return true;
    }

    // Check if cell sprites are used
    if (!CELL_USE_SPRITE) {
        fprintf(stderr, "Cell sprites are not used\n");
        return true;
    }

    // Ensure renderer is valid
    if (!renderer) {
        fprintf(stderr, "Invalid renderer for sprite loading\n");
        return false;
    }

    // Allocate memory safely
    normalSprites = calloc(1, sizeof(CellSprites));
    if (!normalSprites) {
        fprintf(stderr, "Failed to allocate memory for normal sprites\n");
        return false;
    }

    shinySprites = calloc(1, sizeof(CellSprites));
    if (!shinySprites) {
        fprintf(stderr, "Failed to allocate memory for shiny sprites\n");
        free(normalSprites);
        normalSprites = NULL;
        return false;
    }

    char basePath[256];
    char fullPath[512];
    
    // Load normal sprites
    snprintf(basePath, sizeof(basePath), "../ressources/bipboup/normal/");
    
    snprintf(fullPath, sizeof(fullPath), "%sskin.png", basePath);
    normalSprites->skin = IMG_LoadTexture(renderer, fullPath);
    
    snprintf(fullPath, sizeof(fullPath), "%sleaf.png", basePath);
    normalSprites->leaf = IMG_LoadTexture(renderer, fullPath);
    
    snprintf(fullPath, sizeof(fullPath), "%sass.png", basePath);
    normalSprites->ass = IMG_LoadTexture(renderer, fullPath);
    
    snprintf(fullPath, sizeof(fullPath), "%seyes.png", basePath);
    normalSprites->eyes = IMG_LoadTexture(renderer, fullPath);

    // Load shiny sprites
    snprintf(basePath, sizeof(basePath), "../ressources/bipboup/shiny/");
    
    snprintf(fullPath, sizeof(fullPath), "%sskin.png", basePath);
    shinySprites->skin = IMG_LoadTexture(renderer, fullPath);
    
    snprintf(fullPath, sizeof(fullPath), "%sleaf.png", basePath);
    shinySprites->leaf = IMG_LoadTexture(renderer, fullPath);
    
    snprintf(fullPath, sizeof(fullPath), "%sass.png", basePath);
    shinySprites->ass = IMG_LoadTexture(renderer, fullPath);
    
    snprintf(fullPath, sizeof(fullPath), "%seyes.png", basePath);
    shinySprites->eyes = IMG_LoadTexture(renderer, fullPath);

    // Verify loading
    if (!normalSprites->skin || !normalSprites->leaf || !normalSprites->ass || !normalSprites->eyes ||
        !shinySprites->skin || !shinySprites->leaf || !shinySprites->ass || !shinySprites->eyes) {
        fprintf(stderr, "Failed to load one or more cell textures\n");
        free_cell_sprites();
        return false;
    }

    // Mark sprites as loaded
    spritesLoaded = true;
    return true;
}

// Function to check if sprites are loaded
bool are_cell_sprites_loaded() {
    return spritesLoaded;
}

// Function to free sprites when no longer needed
void free_cell_sprites() {
    if (!spritesLoaded) return;

    if (normalSprites) {
        if (normalSprites->skin) SDL_DestroyTexture(normalSprites->skin);
        if (normalSprites->leaf) SDL_DestroyTexture(normalSprites->leaf);
        if (normalSprites->ass) SDL_DestroyTexture(normalSprites->ass);
        if (normalSprites->eyes) SDL_DestroyTexture(normalSprites->eyes);
        free(normalSprites);
        normalSprites = NULL;
    }

    if (shinySprites) {
        if (shinySprites->skin) SDL_DestroyTexture(shinySprites->skin);
        if (shinySprites->leaf) SDL_DestroyTexture(shinySprites->leaf);
        if (shinySprites->ass) SDL_DestroyTexture(shinySprites->ass);
        if (shinySprites->eyes) SDL_DestroyTexture(shinySprites->eyes);
        free(shinySprites);
        shinySprites = NULL;
    }

    spritesLoaded = false;
}

void render_healthbar(Cell *cell, SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, COLOR_WHITE.r, COLOR_WHITE.g, COLOR_WHITE.b, COLOR_WHITE.a);
    SDL_RenderDrawRect(renderer, &(SDL_Rect){cell->position.x - cell->radius, cell->position.y - cell->radius - 10, cell->radius * 2, 5});
    SDL_SetRenderDrawColor(renderer, COLOR_GREEN.r, COLOR_GREEN.g, COLOR_GREEN.b, COLOR_GREEN.a);
    SDL_RenderFillRect(renderer, &(SDL_Rect){cell->position.x - cell->radius, cell->position.y - cell->radius - 10, cell->radius * 2 * (float)cell->health / (float)cell->healthMax, 5});
}

void render_face(Cell *cell, SDL_Renderer *renderer)
{
    // Two small eyes with small arc for the mouth
    int eyeRadius = cell->radius / 8;
    int eyeOffsetX = cell->radius / 1;
    int eyeOffsetY = cell->radius / 2;
    int mouthRadius = cell->radius / 3;
    int mouthOffsetX = cell->radius / 1.5;
    int mouthOffsetY = 0;

    // Rotate eyes around mouth
    int eyeX1 = cell->position.x + eyeOffsetX * cos(cell->angle * PI / 180) - eyeOffsetY * sin(cell->angle * PI / 180);
    int eyeY1 = cell->position.y + eyeOffsetX * sin(cell->angle * PI / 180) + eyeOffsetY * cos(cell->angle * PI / 180);
    int eyeX2 = cell->position.x + eyeOffsetX * cos(cell->angle * PI / 180) + eyeOffsetY * sin(cell->angle * PI / 180);
    int eyeY2 = cell->position.y + eyeOffsetX * sin(cell->angle * PI / 180) - eyeOffsetY * cos(cell->angle * PI / 180);

    int mouthX = cell->position.x + mouthOffsetX * cos(cell->angle * PI / 180) - mouthOffsetY * sin(cell->angle * PI / 180);
    int mouthY = cell->position.y + mouthOffsetX * sin(cell->angle * PI / 180) + mouthOffsetY * cos(cell->angle * PI / 180);

    int mouthStartAngle = 180 + cell->angle - 90;
    int mouthEndAngle = 180 + cell->angle + 90;

    // Draw color
    SDL_SetRenderDrawColor(renderer, COLOR_WHITE.r, COLOR_WHITE.g, COLOR_WHITE.b, COLOR_WHITE.a);

    // Draw eyes
    SDL_RenderFillCircle(renderer, eyeX1, eyeY1, eyeRadius);
    SDL_RenderFillCircle(renderer, eyeX2, eyeY2, eyeRadius);

    // Draw mouth
    SDL_RenderDrawArc(renderer, mouthX, mouthY, mouthRadius, mouthStartAngle, mouthEndAngle);
}

void render_rays(Cell *cell, SDL_Renderer *renderer)
{
    for (int i = 0; i < 7; i++)
    {
        // Set renderer color to ray color
        if (cell->rays[i].distance < cell->rays[i].distanceMax)
        {
            SDL_SetRenderDrawColor(renderer,
                                COLOR_RED.r,
                                COLOR_RED.g,
                                COLOR_RED.b,
                                COLOR_RED.a / 4);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer,
                                COLOR_WHITE.r,
                                COLOR_WHITE.g,
                                COLOR_WHITE.b,
                                COLOR_WHITE.a / 4);
        }

        // Render ray
        SDL_RenderDrawLine(renderer,
                        cell->position.x,
                        cell->position.y,
                        cell->position.x + cell->rays[i].distanceMax * cos(cell->rays[i].angle + cell->angle * PI / 180.0f),
                        cell->position.y + cell->rays[i].distanceMax * sin(cell->rays[i].angle + cell->angle * PI / 180.0f));

        // Render intersection with color based on detected object type
        if (cell->rays[i].hit.type != RAY_OBJECT_NONE)
        {
            // Color based on object type
            switch (cell->rays[i].hit.type)
            {
                case RAY_OBJECT_FOOD:
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green for food
                    break;
                case RAY_OBJECT_CELL:
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red for cells
                    break;
                case RAY_OBJECT_WALL:
                    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // Gray for walls
                    break;
                default:
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White by default
                    break;
            }

            SDL_RenderFillCircle(renderer,
                                cell->position.x + cell->rays[i].distance * cos(cell->rays[i].angle + cell->angle * PI / 180.0f),
                                cell->position.y + cell->rays[i].distance * sin(cell->rays[i].angle + cell->angle * PI / 180.0f),
                                3); // Slightly bigger for better visibility
        }
    }
}

void Cell_render(Cell *cell, SDL_Renderer *renderer, bool renderRays, bool isSelected)
{
    if (!cell->isAlive)
        return;

    // Render based on CELL_USE_SPRITE flag
    if (!CELL_USE_SPRITE) {
        // Simple rendering without sprites
        if (isSelected)
        {
            SDL_SetRenderDrawColor(renderer, COLOR_ORANGE.r, COLOR_ORANGE.g, COLOR_ORANGE.b, COLOR_ORANGE.a);
            SDL_RenderFillCircle(renderer, cell->position.x, cell->position.y, cell->radius);
            render_face(cell, renderer);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, COLOR_VIOLET.r, COLOR_VIOLET.g, COLOR_VIOLET.b, COLOR_VIOLET.a);
            SDL_RenderFillCircle(renderer, cell->position.x, cell->position.y, cell->radius);
            render_face(cell, renderer);
        }
    }
    else {
        // Ensure sprites are loaded
        if (!spritesLoaded) {
            fprintf(stderr, "Sprites not loaded. Call load_all_cell_sprites() first.\n");
            return;
        }

        // Use shiny sprites for selected cell (best score)
        CellSprites *sprites = isSelected ? shinySprites : normalSprites;

        // Calculate angle in radians
        float rad = cell->angle * PI / 180.0f;

        // Determine texture to display: if angle is less than 180°, cell is facing down (show eyes)
        // otherwise it's facing up (show back "ass")
        bool showEyes = (cell->angle < 180);

        // Calculate horizontal offset based on horizontal component
        // When cell->angle == 0, cos(0)=1 (max positive offset), and for cell->angle == 180, cos(180)=-1 (max negative offset)
        float tOffset = (cos(rad) + 1.0f) / 2.0f;
        float maxOffset = cell->radius * 0.4f;
        int offsetX = (int)(((tOffset - 0.5f) * 2.0f) * maxOffset);

        int radius = cell->radius * 1.5;
        SDL_Rect destRect = {
            cell->position.x - radius,
            cell->position.y - radius,
            radius * 2,
            radius * 2
        };

        SDL_RendererFlip flip = SDL_FLIP_NONE;

        // Display skin, always present
        SDL_RenderCopyEx(renderer, sprites->skin, NULL, &destRect, 0, NULL, flip);

        if (showEyes) {
            // When cell is facing down (angle < 180°), display eyes
            SDL_Rect eyesRect = destRect;
            eyesRect.x += offsetX;
            SDL_RenderCopyEx(renderer, sprites->eyes, NULL, &eyesRect, 0, NULL, flip);
        } else {
            // When cell is facing up (angle >= 180°), display back (ass) with inverted offset
            SDL_Rect assRect = destRect;
            assRect.x -= offsetX;
            SDL_RenderCopyEx(renderer, sprites->ass, NULL, &assRect, 0, NULL, flip);
        }

        // Always display leaf on top
        SDL_RenderCopyEx(renderer, sprites->leaf, NULL, &destRect, 0, NULL, flip);
    }

    render_healthbar(cell, renderer);

    if (renderRays)
        render_rays(cell, renderer);
}
