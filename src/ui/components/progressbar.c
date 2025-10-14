#include "../../../include/ui/components/progressbar.h"
#include "../../../include/ui/ui_utils.h"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>

void ProgressBar_Render(SDL_Renderer *renderer, int x, int y, int width, int height,
                       float progress, const char* label, SDL_Color bgColor, SDL_Color fgColor)
{
    int cornerRadius = 4;

    // Clamp progress between 0 and 1
    progress = fmaxf(0.0f, fminf(1.0f, progress));

    // Draw background
    DrawSmoothRoundedRect(renderer, x, y, width, height, cornerRadius, bgColor);

    // Draw foreground (progress)
    int fgWidth = (int)(width * progress);
    if (fgWidth > 0) {
        DrawSmoothRoundedRect(renderer, x, y, fgWidth, height, cornerRadius, fgColor);
    }

    // Draw label
    if (label && label[0] != '\0') {
        SDL_Color labelColor = {200, 200, 200, 255};
        stringRGBA(renderer, x, y - height - 8, label, labelColor.r, labelColor.g, labelColor.b, labelColor.a);
    }
}
