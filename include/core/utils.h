#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <SDL2/SDL.h>

// MacOS
#if defined(__APPLE__)
    #include <SDL_image.h>
#else
// Linux
    #include <SDL2/SDL_image.h>
#endif


// Define MAX and MIN macros
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define CLAMP(X, Y, Z) (MAX(MIN(X, Z), Y))
#define CLAMP01(X) CLAMP(X, 0.0, 1.0)

#define COLOR_BREAKUP(COLOR)    COLOR.r, COLOR.g, COLOR.b, COLOR.a


extern const SDL_Color COLOR_TRANSPARENT;
extern const SDL_Color COLOR_BLACK;
extern const SDL_Color COLOR_WHITE;
extern const SDL_Color COLOR_GRAY;
extern const SDL_Color COLOR_DARK_GRAY;
extern const SDL_Color COLOR_DARK_GREEN;
extern const SDL_Color COLOR_LIGHT_GRAY;
extern const SDL_Color COLOR_RED;
extern const SDL_Color COLOR_GREEN;
extern const SDL_Color COLOR_BLUE;
extern const SDL_Color COLOR_YELLOW;
extern const SDL_Color COLOR_ORANGE;
extern const SDL_Color COLOR_PINK;
extern const SDL_Color COLOR_VIOLET;


void Utils_setBackgroundColor(SDL_Renderer *renderer, SDL_Color color);

void Utils_randInit(void);
float Utils_map(float value, float min1, float max1, float min2, float max2);
int irand(int min, int max);
double drand(double min, double max);

#endif // UTILS_H
