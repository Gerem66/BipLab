#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <SDL.h>
#include <SDL2/SDL_image.h>


// Define MAX and MIN macros
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define CLAMP(X, Y, Z) (MAX(MIN(X, Z), Y))

#define COLOR_BREAKUP(COLOR)    COLOR.r, COLOR.g, COLOR.b, COLOR.a

#define PI 3.14159265358979323846


extern const SDL_Color COLOR_TRANSPARENT;
extern const SDL_Color COLOR_BLACK;
extern const SDL_Color COLOR_WHITE;
extern const SDL_Color COLOR_GRAY;
extern const SDL_Color COLOR_DARK_GRAY;
extern const SDL_Color COLOR_LIGHT_GRAY;
extern const SDL_Color COLOR_RED;
extern const SDL_Color COLOR_GREEN;
extern const SDL_Color COLOR_BLUE;
extern const SDL_Color COLOR_YELLOW;
extern const SDL_Color COLOR_ORANGE;
extern const SDL_Color COLOR_PINK;
extern const SDL_Color COLOR_VIOLET;


void Utils_setBackgroundColor(SDL_Renderer *renderer, SDL_Color color);

long long Utils_time();
float GetMillisecondsElapsed();

void Utils_randInit();
int Utils_rand(int min, int max);
float Utils_map(float value, float min1, float max1, float min2, float max2);
float Utils_randomFloat(float min, float max);

void SDL_RenderFillCircle(SDL_Renderer* rend, int x0, int y0, int radius);
void SDL_RenderDrawArc(SDL_Renderer* rend, int x0, int y0, int radius, int startAngle, int endAngle);
void SDL_RenderDrawCircle(SDL_Renderer *renderer, int x, int y, int radius);
void SDL_RenderDrawCircleOutline(SDL_Renderer *renderer, int x, int y, int radius);

SDL_Texture *LoadSprite(SDL_Renderer *renderer, const char *file);
void freeSprite(SDL_Texture *sprite);

#endif // UTILS_H
