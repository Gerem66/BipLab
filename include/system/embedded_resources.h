#ifndef EMBEDDED_RESOURCES_H
#define EMBEDDED_RESOURCES_H

#include <stddef.h>
#include <SDL2/SDL.h>

// Resource identifier enum
typedef enum {
    RES_SKIN_NORMAL,
    RES_EYES_NORMAL,
    RES_LEAF_NORMAL,
    RES_ASS_NORMAL,
    RES_SKIN_SHINY,
    RES_EYES_SHINY,
    RES_LEAF_SHINY,
    RES_ASS_SHINY,
    RES_COUNT // Always keep this last - it gives us the total count
} ResourceId;

// Resource structure
typedef struct {
    const char* data;
    size_t size;
} EmbeddedResource;

// Simple API - just get any resource by ID
EmbeddedResource get_embedded_resource(ResourceId id);

// Helper to load texture from embedded data
SDL_Texture* load_texture_from_embedded_data(SDL_Renderer* renderer, const char* data, size_t size);

#endif // EMBEDDED_RESOURCES_H
