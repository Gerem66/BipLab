#include "../../../include/ui/components/button.h"
#include "../../../include/ui/ui_utils.h"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <string.h>

void Button_Init(Button *button, int x, int y, int width, int height, const char *label)
{
    button->rect.x = x;
    button->rect.y = y;
    button->rect.w = width;
    button->rect.h = height;

    strncpy(button->label, label, sizeof(button->label) - 1);
    button->label[sizeof(button->label) - 1] = '\0';

    button->isPressed = false;
    button->isHovered = false;

    // Default colors
    button->bgColor = (SDL_Color){60, 60, 80, 255};
    button->textColor = (SDL_Color){255, 255, 255, 255};
    button->hoverColor = (SDL_Color){80, 80, 100, 255};
}

bool Button_Update(Button *button, int mouseX, int mouseY, bool mousePressed)
{
    // Check if mouse is inside button bounds
    bool isInside = (mouseX >= button->rect.x && mouseX <= button->rect.x + button->rect.w &&
                     mouseY >= button->rect.y && mouseY <= button->rect.y + button->rect.h);

    button->isHovered = isInside;

    // Detect click (press inside button)
    bool clicked = false;
    if (isInside && mousePressed) {
        clicked = true;
        button->isPressed = true;
    } else {
        button->isPressed = false;
    }

    return clicked;
}

void Button_Render(SDL_Renderer *renderer, Button *button)
{
    SDL_Color currentBgColor = button->isHovered ? button->hoverColor : button->bgColor;

    // Darken color if pressed
    if (button->isPressed) {
        currentBgColor.r = (Uint8)(currentBgColor.r * 0.8f);
        currentBgColor.g = (Uint8)(currentBgColor.g * 0.8f);
        currentBgColor.b = (Uint8)(currentBgColor.b * 0.8f);
    }

    // Draw button background with rounded corners
    int cornerRadius = 4;
    DrawSmoothRoundedRect(renderer, button->rect.x, button->rect.y,
                         button->rect.w, button->rect.h, cornerRadius, currentBgColor);

    // Draw centered text
    int textX = button->rect.x + button->rect.w / 2 - (int)(strlen(button->label) * 4); // Approximation
    int textY = button->rect.y + button->rect.h / 2 - 4; // Text height approximation

    stringRGBA(renderer, textX, textY, button->label,
               button->textColor.r, button->textColor.g, button->textColor.b, button->textColor.a);
}
