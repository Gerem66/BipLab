#include "utils.h"

#include <time.h>


const SDL_Color COLOR_TRANSPARENT   = {0};
const SDL_Color COLOR_BLACK         = {0, 0, 0, 255};
const SDL_Color COLOR_WHITE         = {255, 255, 255, 255};
const SDL_Color COLOR_GRAY          = {100, 100, 100, 255};
const SDL_Color COLOR_DARK_GRAY     = {30, 30, 30, 255};
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

long long Utils_time()
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC_RAW, &t);
    return t.tv_sec * 1000 + t.tv_nsec / 1000000;
}

float GetMillisecondsElapsed()
{
	static long long previous = 0;
	long long current = Utils_time();
	float elapsed = (current - previous) / 1000.0f;
	previous = current;
	return elapsed;
}

void Utils_randInit()
{
    srand(time(NULL));
}

int Utils_rand(int min, int max)
{
    return (rand() % (max + 1)) + min;
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
