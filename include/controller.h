#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <SDL2/SDL.h>
#include "model.h"

// Function declarations for handling user input and events
void handleEvents(bool *closeWindow, MovementFlags flags[], int numPlayers);
void handleMenuEvent(SDL_Event* e, GameState* gameState);

#endif /* CONTROLLER_H */
