#ifndef BUTTON_H
#define BUTTON_H

#include <SDL2/SDL.h>
#include <stdbool.h>

// Structure to represent a button
typedef struct {
    SDL_Rect rect;        // Button position and size
    char label[64];       // Button text
    bool isPressed;       // Pressed state
    bool isHovered;       // Hovered state
    SDL_Color bgColor;    // Background color
    SDL_Color textColor;  // Text color
    SDL_Color hoverColor; // Hover color
} Button;

/**
 * Initialize a button
 * @param button Pointer to the button to initialize
 * @param x X position
 * @param y Y position
 * @param width Width
 * @param height Height
 * @param label Button text
 */
void Button_Init(Button *button, int x, int y, int width, int height, const char *label);

/**
 * Update button state based on mouse position
 * @param button Pointer to the button
 * @param mouseX Mouse X position
 * @param mouseY Mouse Y position
 * @param mousePressed Mouse button state
 * @return true if button was clicked
 */
bool Button_Update(Button *button, int mouseX, int mouseY, bool mousePressed);

/**
 * Render the button
 * @param renderer SDL renderer
 * @param button Pointer to the button
 */
void Button_Render(SDL_Renderer *renderer, Button *button);

#endif // BUTTON_H
