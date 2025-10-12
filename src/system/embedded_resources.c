#include "../../include/system/embedded_resources.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// Declare symbols from linker - clean macro approach
#define DECLARE_RESOURCE(name) \
    extern char _binary__home_gerem_SynologyDrive_Projects_BipLab_ressources_bipboup_##name##_png_start[]; \
    extern char _binary__home_gerem_SynologyDrive_Projects_BipLab_ressources_bipboup_##name##_png_end[]

DECLARE_RESOURCE(normal_skin);
DECLARE_RESOURCE(normal_eyes);
DECLARE_RESOURCE(normal_leaf);
DECLARE_RESOURCE(normal_ass);
DECLARE_RESOURCE(shiny_skin);
DECLARE_RESOURCE(shiny_eyes);
DECLARE_RESOURCE(shiny_leaf);
DECLARE_RESOURCE(shiny_ass);

// Resource mapping - just store pointers, calculate size dynamically
typedef struct {
    char* start;
    char* end;
} ResourceMapping;

#define RESOURCE_MAPPING(name) { \
    .start = _binary__home_gerem_SynologyDrive_Projects_BipLab_ressources_bipboup_##name##_png_start, \
    .end = _binary__home_gerem_SynologyDrive_Projects_BipLab_ressources_bipboup_##name##_png_end \
}

// Resource mapping table - ADD/REMOVE RESOURCES HERE ONLY
static const ResourceMapping resource_mappings[RES_COUNT] = {
    [RES_SKIN_NORMAL] = RESOURCE_MAPPING(normal_skin),
    [RES_EYES_NORMAL] = RESOURCE_MAPPING(normal_eyes),
    [RES_LEAF_NORMAL] = RESOURCE_MAPPING(normal_leaf),
    [RES_ASS_NORMAL]  = RESOURCE_MAPPING(normal_ass),
    [RES_SKIN_SHINY]  = RESOURCE_MAPPING(shiny_skin),
    [RES_EYES_SHINY]  = RESOURCE_MAPPING(shiny_eyes),
    [RES_LEAF_SHINY]  = RESOURCE_MAPPING(shiny_leaf),
    [RES_ASS_SHINY]   = RESOURCE_MAPPING(shiny_ass),
};

// Simple API - get any resource by ID
EmbeddedResource get_embedded_resource(ResourceId id) {
    if (id >= 0 && id < RES_COUNT) {
        const ResourceMapping* mapping = &resource_mappings[id];
        return (EmbeddedResource){
            .data = mapping->start,
            .size = (size_t)(mapping->end - mapping->start)
        };
    }
    return (EmbeddedResource){.data = NULL, .size = 0};
}

// Helper function to load texture from embedded data
SDL_Texture* load_texture_from_embedded_data(SDL_Renderer* renderer, const char* data, size_t size) {
    if (!data || size == 0) return NULL;

    SDL_RWops* rw = SDL_RWFromConstMem(data, (int)size);
    if (!rw) return NULL;

    SDL_Surface* surface = IMG_Load_RW(rw, 1);
    if (!surface) return NULL;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    return texture;
}
