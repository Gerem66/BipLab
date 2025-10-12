#include "../../include/ui/ui_utils.h"
#include <math.h>

// Helper function to draw pixel-perfect antialiased rounded rectangles
void DrawSmoothRoundedRect(SDL_Renderer *renderer, int x, int y, int w, int h, int radius, SDL_Color color) {
    if (w <= 0 || h <= 0) return;

    // Clamp radius to reasonable values
    if (radius > h/2) radius = h/2;
    if (radius > w/2) radius = w/2;
    if (radius < 0) radius = 0;

    if (radius <= 1) {
        // For very small radius, just draw a simple rectangle
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_Rect rect = {x, y, w, h};
        SDL_RenderFillRect(renderer, &rect);
        return;
    }

    // Draw the main rectangles (avoiding corners)
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // Center rectangle (full width, excluding top and bottom radius)
    SDL_Rect centerRect = {x, y + radius, w, h - 2 * radius};
    SDL_RenderFillRect(renderer, &centerRect);

    // Top and bottom rectangles (excluding corners)
    SDL_Rect topRect = {x + radius, y, w - 2 * radius, radius};
    SDL_Rect bottomRect = {x + radius, y + h - radius, w - 2 * radius, radius};
    SDL_RenderFillRect(renderer, &topRect);
    SDL_RenderFillRect(renderer, &bottomRect);

    // Draw corners with manual antialiasing
    for (int corner = 0; corner < 4; corner++) {
        int cx, cy; // Corner center
        switch (corner) {
            case 0: cx = x + radius; cy = y + radius; break; // Top-left
            case 1: cx = x + w - radius; cy = y + radius; break; // Top-right
            case 2: cx = x + radius; cy = y + h - radius; break; // Bottom-left
            case 3: cx = x + w - radius; cy = y + h - radius; break; // Bottom-right
        }

        // Draw a filled circle for the corner using individual pixels with antialiasing
        for (int py = cy - radius; py < cy + radius; py++) {
            for (int px = cx - radius; px < cx + radius; px++) {
                float dx = px - cx + 0.5f;
                float dy = py - cy + 0.5f;
                float distance = sqrtf(dx*dx + dy*dy);

                if (distance <= radius) {
                    // Calculate antialiasing alpha
                    float alpha = 1.0f;
                    if (distance > radius - 1.0f) {
                        alpha = radius - distance; // Smooth falloff at edge
                    }

                    if (alpha > 0.0f) {
                        Uint8 finalAlpha = (Uint8)(color.a * alpha);
                        pixelRGBA(renderer, px, py, color.r, color.g, color.b, finalAlpha);
                    }
                }
            }
        }
    }
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
    // Convert angles to radians
    double startRad = startAngle * (PI / 180.0);
    double endRad = endAngle * (PI / 180.0);

    // Calculate step angle for each iteration
    double step = PI / (2.0 * radius); // The larger the radius, the smaller the step can be

    // Draw arc point by point
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
