#include "popup.h"

int open_popup_ask(char *title, char *message)
{
    SDL_MessageBoxData messageboxdata;
    messageboxdata.flags = SDL_MESSAGEBOX_INFORMATION;
    messageboxdata.window = NULL;
    messageboxdata.title = title;
    messageboxdata.message = message;
    messageboxdata.numbuttons = 2;
    messageboxdata.buttons = (SDL_MessageBoxButtonData[2]) {
        { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "No" },
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Yes" },
    };
    messageboxdata.colorScheme = NULL;

    int buttonid;
    if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0)
    {
        fprintf(stderr, "Error while showing message box !\n");
        return -1;
    }
    return buttonid;
}

int open_popup_message(char *title, char *message)
{
    SDL_MessageBoxData messageboxdata;
    messageboxdata.flags = SDL_MESSAGEBOX_INFORMATION;
    messageboxdata.window = NULL;
    messageboxdata.title = title;
    messageboxdata.message = message;
    messageboxdata.numbuttons = 1;
    messageboxdata.buttons = (SDL_MessageBoxButtonData[1]) {
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "OK" },
    };
    messageboxdata.colorScheme = NULL;

    int buttonid;
    if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0)
    {
        fprintf(stderr, "Error while showing message box !\n");
        return -1;
    }
    return buttonid;
}