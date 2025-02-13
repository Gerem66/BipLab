#include "utils.h"

#include <time.h>


const SDL_Color COLOR_TRANSPARENT   = {0};
const SDL_Color COLOR_BLACK         = {0, 0, 0, 255};
const SDL_Color COLOR_WHITE         = {255, 255, 255, 255};
const SDL_Color COLOR_GRAY          = {100, 100, 100, 255};
const SDL_Color COLOR_DARK_GRAY     = {30, 30, 30, 255};
const SDL_Color COLOR_DARK_GREEN     = {5, 51, 41, 255};
const SDL_Color COLOR_LIGHT_GRAY    = {200, 200, 200, 255};
const SDL_Color COLOR_RED           = {255, 0, 0, 255};
const SDL_Color COLOR_GREEN         = {0, 255, 0, 255};
const SDL_Color COLOR_BLUE          = {0, 0, 255, 255};
const SDL_Color COLOR_YELLOW        = {255, 255, 0, 255};
const SDL_Color COLOR_ORANGE        = {255, 133, 0, 255};
const SDL_Color COLOR_PINK          = {255, 0, 206, 255};
const SDL_Color COLOR_VIOLET        = {145, 0, 255, 255};

void Utils_setBackgroundColor(SDL_Renderer *renderer, SDL_Color color)
{
    // Initialize renderer color
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // Clear screen
    SDL_RenderClear(renderer);
}

void Utils_randInit(void)
{
    srand(time(NULL));
}

int Utils_rand(int min, int max)
{
    return (rand() % (max + 1)) + min;
}

float Utils_map(float value, float min1, float max1, float min2, float max2)
{
	return (value - min1) / (max1 - min1) * (max2 - min2) + min2;
}

float Utils_randomFloat(float min, float max)
{
	return ((float)rand() / (float)RAND_MAX) * (max - min) + min;
}

// Draw a filled circle on the renderer
// https://gist.github.com/henkman/1b6f4492b82dc76adad1dc110c923baa
void SDL_RenderFillCircle(SDL_Renderer* rend, int x0, int y0, int radius)
{
	// Uses the midpoint circle algorithm to draw a filled circle
	// https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
	int x = radius;
	int y = 0;
	int radiusError = 1 - x;
	while (x >= y) {
		SDL_RenderDrawLine(rend, x + x0, y + y0, -x + x0, y + y0);
		SDL_RenderDrawLine(rend, y + x0, x + y0, -y + x0, x + y0);
		SDL_RenderDrawLine(rend, -x + x0, -y + y0, x + x0, -y + y0);
		SDL_RenderDrawLine(rend, -y + x0, -x + y0, y + x0, -x + y0);
		y++;
		if (radiusError < 0)
			radiusError += 2 * y + 1;
		else {
			x--;
			radiusError += 2 * (y - x + 1);
		}
	}
}

// By ChatGPT
void SDL_RenderDrawArc(SDL_Renderer* rend, int x0, int y0, int radius, int startAngle, int endAngle) {
    // Conversion des angles en radians
    double startRad = startAngle * (PI / 180.0);
    double endRad = endAngle * (PI / 180.0);

    // Calcul de l'angle d'étape pour chaque itération
    double step = PI / (2.0 * radius); // Plus le rayon est grand, plus l'étape peut être petite

    // Dessiner l'arc point par point
    for (double theta = startRad; theta < endRad; theta += step) {
        int x = x0 + radius * cos(theta);
        int y = y0 + radius * sin(theta);

        SDL_RenderDrawPoint(rend, x, y);
    }
}

// By ChatGPT
void SDL_RenderDrawCircle(SDL_Renderer *renderer, int x, int y, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx*dx + dy*dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

void SDL_RenderDrawCircleOutline(SDL_Renderer *renderer, int x, int y, int radius) {
    SDL_RenderDrawArc(renderer, x, y, radius, 0, 360);
    SDL_RenderDrawArc(renderer, x, y, radius - 1, 0, 360);
}

SDL_Texture *LoadSprite(SDL_Renderer *renderer, const char *file)
{
    // Load cell sprite
    SDL_Surface *surface = IMG_Load(file);
    if (surface == NULL)
    {
        printf("Erreur de chargement de l'image : %s\n", SDL_GetError());
        return NULL;
    }

    SDL_Texture *sprite = SDL_CreateTextureFromSurface(renderer, surface);
    if (sprite == NULL)
    {
        printf("Erreur de création de la texture : %s\n", SDL_GetError());
        return NULL;
    }
    SDL_FreeSurface(surface);

    return sprite;
}

void freeSprite(SDL_Texture *sprite)
{
    SDL_DestroyTexture(sprite);
}
