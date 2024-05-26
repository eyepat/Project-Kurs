#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_ttf.h>
#include "model.h"

// Function declarations for handling user input and events
// void handleHostEvents(bool *closeWindow, Client clients[], GameState *gameState, int isServer);
void handleEvents(bool *closeWindow, Client clients[], GameState *gameState, int isServer, Client *myClientInfo);

void handleClientEvents(bool *closeWindow, Client *myClientInfo);
void handleMenuEvent(bool *closeWindow, MenuState* menuState);
void cleanup(GameState *gameState, SDL_Texture *fieldTexture, SDL_Renderer *renderer, SDL_Window *window, TTF_Font *font, Client clients[], Client *myClientInfo, SDLNet_SocketSet socketSet);

#endif /* CONTROLLER_H */