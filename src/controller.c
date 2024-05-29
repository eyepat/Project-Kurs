#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "controller.h"

void handleEvents(bool *closeWindow, Client clients[], GameState *gameState, int isServer, Client *myClientInfo) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *closeWindow = true;
            return;
        }
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            // Determine the value based on whether the event is key down or key up
            bool value = (event.type == SDL_KEYDOWN);

            // Host uses WASD keys for client[0]
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_W:
                        clients[0].flags.up = value;
                        break;
                    case SDL_SCANCODE_S:
                        clients[0].flags.down = value;
                        break;
                    case SDL_SCANCODE_A:
                        clients[0].flags.left = value;
                        break;
                    case SDL_SCANCODE_D:
                        clients[0].flags.right = value;
                        break;
                    default:
                        break;
                }

             if (isServer == 2) {
                myClientInfo->flags = clients[0].flags;
            }

            if (isServer == 0) { // arrows for client[1]
                switch (event.key.keysym.scancode) {
                    // Arrow keys for client[1]
                    case SDL_SCANCODE_UP:
                        clients[1].flags.up = value;
                        break;
                    case SDL_SCANCODE_DOWN:
                        clients[1].flags.down = value;
                        break;
                    case SDL_SCANCODE_LEFT:
                        clients[1].flags.left = value;
                        break;
                    case SDL_SCANCODE_RIGHT:
                        clients[1].flags.right = value;
                        break;
                    default:
                        break;
                }
            }
        }
    }
}


void handleMenuEvent (bool *closeWindow, MenuState* menuState) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            menuState->menuState = 77;
            *closeWindow = true;
            // return;
        }
        else if (e.type == SDL_MOUSEBUTTONDOWN) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            SDL_Point mousePos = {x, y};

            if (menuState->menuState == 0 && SDL_PointInRect(&mousePos, &menuState->onlineButton.bounds)) {
                printf("Online button clicked.\n");
                menuState->menuState = 1;//online menu
            } 
            else if (menuState->menuState == 0 && SDL_PointInRect(&mousePos, &menuState->localButton.bounds)) {
                printf("Local button clicked.\n");
                menuState->menuState = 2;//start local two player game
            } 
            else if (SDL_PointInRect(&mousePos, &menuState->exitButton.bounds)) {
                printf("Exit button clicked.\n");
                menuState->menuState = 77;
                *closeWindow = true; // Set closeWindow to true to exit the game
            }
            else if (menuState->menuState == 1 && SDL_PointInRect(&mousePos, &menuState->hostButton.bounds)) {
                // Add the action for the host button
                printf("Host button clicked.\n");
                menuState->menuState = 3;//host menu
                SDL_DestroyTexture(menuState->hostButton.texture);
                SDL_DestroyTexture(menuState->joinButton.texture);
                SDL_DestroyTexture(menuState->exitButton.texture);
            } 
            else if (menuState->menuState == 1 && SDL_PointInRect(&mousePos, &menuState->joinButton.bounds)) {
                printf("join button clicked.\n");
                menuState->menuState = 4;//join menu
                SDL_DestroyTexture(menuState->hostButton.texture);
                SDL_DestroyTexture(menuState->exitButton.texture);
                SDL_DestroyTexture(menuState->joinButton.texture);
            }
            else if (menuState->menuState == 3 && SDL_PointInRect(&mousePos, &menuState->startButton.bounds)) {
                printf("start button clicked.\n");
                menuState->menuState = 6;//start game
            }
        }
    }
}

void cleanup(GameState *gameState, SDL_Texture *fieldTexture, SDL_Renderer *renderer, SDL_Window *window, TTF_Font *font, Client clients[], Client *myClientInfo, SDLNet_SocketSet socketSet) {
    // Clean up SDL objects
    SDL_DestroyTexture(fieldTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    Mix_CloseAudio();
    destroyTimer(gameState->gameTimer);

    // Clean up client sockets array and myClientInfo socket
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (clients[i].socket != NULL) {
            SDLNet_TCP_Close(clients[i].socket);
            clients[i].socket = NULL;
        }
    }
    if (myClientInfo != NULL && myClientInfo->socket != NULL) {
        SDLNet_TCP_Close(myClientInfo->socket);
        myClientInfo->socket = NULL;
    }
    // Clean up SDLNet socket set
    SDLNet_FreeSocketSet(socketSet);
    socketSet = NULL;
    SDLNet_Quit();
    TTF_CloseFont(font);
    SDL_Quit();
}
