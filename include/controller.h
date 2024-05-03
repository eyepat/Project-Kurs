#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_ttf.h>
#include "model.h"

// Function declarations for handling user input and events
void handleEvents(bool *closeWindow, MovementFlags flags[], GameState *gameState);
void handleMenuEvent(bool *closeWindow, MenuState* menuState);

#endif /* CONTROLLER_H */