#include "controller.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

void handleHostEvents(bool *closeWindow, Client clients[], GameState *gameState) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *closeWindow = true;
            return;
        }
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            // Determine the value based on whether the event is key down or key up
            bool value = (event.type == SDL_KEYDOWN);
            switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_W:
                    clients[0].flags.up = value; // Update the movement flags for the host only
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
        }
    }
}

void handleClientEvents(bool *closeWindow, Client *myClientInfo) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *closeWindow = true;
            return;
        }
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            // Determine the value based on whether the event is key down or key up
            bool value = (event.type == SDL_KEYDOWN);
            switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_W:
                    myClientInfo->flags.up = value;
                    break;
                case SDL_SCANCODE_S:
                    myClientInfo->flags.down = value;
                    break;
                case SDL_SCANCODE_A:
                    myClientInfo->flags.left = value;
                    break;
                case SDL_SCANCODE_D:
                    myClientInfo->flags.right = value;
                    break;
                default:
                    break;
            }
        }
    }
}



void handleMenuEvent (bool *closeWindow, MenuState* menuState) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            *closeWindow = true;
            return;
        }
        else if (e.type == SDL_MOUSEBUTTONDOWN) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            SDL_Point mousePos = {x, y};
              
            //menuState->menuState = 0;//start menu, choose to play online or local
            //menuState->menuState = 1;//online menu
            //menuState->menuState = 2;//start local two player game
            //menuState->menuState = 3;//host menu
            //menuState->menuState = 4;//join menu
            //menuState->menuState = 5;//exit game
            //menuState->menuState = 6;//start game
            //menuState->menuState = 7;//join host
            //menuState->menuState = 8;//enter ip
            //startButton position as exit
            //joinHostButton position as exit
            //onlineButton position as host
            //localButton same position as join
            //ipInputButton same position as host

            if (menuState->menuState == 0 && SDL_PointInRect(&mousePos, &menuState->onlineButton.bounds)) {
                // Add the action for the host button
                printf("Online button clicked.\n");
                menuState->menuState = 1;//online menu
            } 
            else if (menuState->menuState == 0 && SDL_PointInRect(&mousePos, &menuState->localButton.bounds)) {
                // Add the action for the host button
                printf("Local button clicked.\n");
                menuState->menuState = 2;//start local two player game
            } 
            else if (SDL_PointInRect(&mousePos, &menuState->exitButton.bounds)) {
                    printf("Exit button clicked.\n");
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
                // Add the action for the join button here
                printf("join button clicked.\n");
                  menuState->menuState = 4;//join menu
                SDL_DestroyTexture(menuState->hostButton.texture);
                SDL_DestroyTexture(menuState->exitButton.texture);
                SDL_DestroyTexture(menuState->joinButton.texture);
            }
            else if (menuState->menuState == 3 && SDL_PointInRect(&mousePos, &menuState->startButton.bounds)) {
                // Add the action for the start button 
                printf("start button clicked.\n");
                menuState->menuState = 6;//start game
            }
        }
    }
}


void cleanup(SDL_Texture *fieldTexture, SDL_Renderer *renderer, SDL_Window *window, TTF_Font *font, Client clients[], Client *myClientInfo, SDLNet_SocketSet socketSet) {
  
    // Clean up SDL objects
    SDL_DestroyTexture(fieldTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    Mix_CloseAudio();

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

void localControls(bool *closeWindow, GameState *gameState, MovementFlags playerMovement[2]) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *closeWindow = true;
            return;
        }
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            // Determine the value based on whether the event is key down or key up
            bool value = (event.type == SDL_KEYDOWN); 
            switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_W:
                    playerMovement[0].up = value;
                    break;
                case SDL_SCANCODE_S:
                    playerMovement[0].down = value;
                    break;
                case SDL_SCANCODE_A:
                    playerMovement[0].left = value;
                    break;
                case SDL_SCANCODE_D:
                    playerMovement[0].right = value;
                    break;
                case SDL_SCANCODE_DOWN:
                    playerMovement[1].down = value;
                    break;
                case SDL_SCANCODE_UP:
                    playerMovement[1].up = value;
                    break;
                case SDL_SCANCODE_LEFT:
                    playerMovement[1].left = value;
                    break;
                case SDL_SCANCODE_RIGHT:
                    playerMovement[1].right = value;
                    break;
                default:
                    break;
            
            
            
            }
        }
    }
}