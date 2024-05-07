#include "controller.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

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
                    printf("W hostevents\n");
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
            if (SDL_PointInRect(&mousePos, &menuState->hostButton.bounds)) {
                // Add the action for the host button
                printf("Host button clicked.\n");
                menuState->menuState = 1;//host menu
                SDL_DestroyTexture(menuState->hostButton.texture);
                SDL_DestroyTexture(menuState->joinButton.texture);
                SDL_DestroyTexture(menuState->exitButton.texture);
            } 
            else if (SDL_PointInRect(&mousePos, &menuState->joinButton.bounds)) {
                // Add the action for the join button here
                printf("join button clicked.\n");
                menuState->menuState = 2;//join menu
                SDL_DestroyTexture(menuState->hostButton.texture);
                SDL_DestroyTexture(menuState->exitButton.texture);
                SDL_DestroyTexture(menuState->joinButton.texture);
            } 
            else if (SDL_PointInRect(&mousePos, &menuState->exitButton.bounds)) {
                // Add the action for the exit button here
                printf("exit button clicked.\n");
                menuState->menuState = 3;//exit game
            }
            else if (SDL_PointInRect(&mousePos, &menuState->startButton.bounds)) {
                // Add the action for the start button 
                printf("start button clicked.\n");
                menuState->menuState = 4;//start game
            }
            else if (SDL_PointInRect(&mousePos, &menuState->joinHostButton.bounds)) {
                // Add the action for the join button 
                printf("join host button clicked.\n");
                menuState->menuState = 5;//join host
            }
        }
    }
}