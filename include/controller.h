#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_ttf.h>
#include "model.h"

// Function declarations for handling user input and events
void handleEvents(bool *closeWindow, MovementFlags flags[], GameState *gameState);
void handleMenuEvent(bool *closeWindow, MenuState* menuState);
void cleanup(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* fieldTexture, TTF_Font* font, TCPsocket serverSocket, TCPsocket clientSockets[], SDLNet_SocketSet socketSet);

#endif /* CONTROLLER_H */
