#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_ttf.h>
#include "model.h"

// Function declarations for handling user input and events

void handleHostEvents(bool *closeWindow, Client clients[], GameState *gameState);
void handleClientEvents(bool *closeWindow, Client *myClientInfo);


void handleMenuEvent(bool *closeWindow, MenuState* menuState);

#endif /* CONTROLLER_H */