#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_ttf.h>
#include "model.h"
#include "view.h"

// Function declarations for handling user input and events
void handleEvents(bool *closeWindow, Client clients[], GameState *gameState, int isServer, Client *myClientInfo);
void handleMenuEvent(bool *closeWindow, MenuState* menuState);
void handleUserInput(SDL_Renderer* renderer, TTF_Font* font, MenuState* menuState, int* portPointer, const char* prompt, int windowWidth);
void handleGameOver(bool *closeWindow, GameState *gameState, SDL_Renderer *renderer, TTF_Font *font, Field *field, int isServer, Client clients[], SDLNet_SocketSet socketSet);
void cleanup(GameState *gameState, SDL_Texture *fieldTexture, SDL_Renderer *renderer, SDL_Window *window, TTF_Font *font, Client clients[], Client *myClientInfo, SDLNet_SocketSet socketSet);

#endif /* CONTROLLER_H */