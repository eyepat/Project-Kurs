#include "controller.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

void handleEvents(bool *closeWindow, MovementFlags flags[], int numPlayers) {
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
                    flags[0].up = value;
                    break;
                case SDL_SCANCODE_UP:
                    flags[1].up = value;
                    break;
                case SDL_SCANCODE_S:
                    flags[0].down = value;
                    break;
                case SDL_SCANCODE_DOWN:
                    flags[1].down = value;
                    break;
                case SDL_SCANCODE_A:
                    flags[0].left = value;
                    break;
                case SDL_SCANCODE_LEFT:
                    flags[1].left = value;
                    break;
                case SDL_SCANCODE_D:
                    flags[0].right = value;
                    break;
                case SDL_SCANCODE_RIGHT:
                    flags[1].right = value;
                    break;
                
            }
        }
    }
}


void handleMenuEvent(SDL_Event* e, GameState* gameState) {
    if (e->type == SDL_MOUSEBUTTONDOWN) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        SDL_Point mousePos = {x, y};
        if (SDL_PointInRect(&mousePos, &gameState->hostButton.bounds)) {
            // Add the action for the host button
            printf("Host button clicked.\n");
            gameState->menuState = 11;
            SDL_DestroyTexture(gameState->hostButton.texture);
            SDL_DestroyTexture(gameState->joinButton.texture);
            SDL_DestroyTexture(gameState->exitButton.texture);
            if (SDL_PointInRect(&mousePos, &gameState->startButton.bounds)) {
            // Add the action for the start button 
            printf("start button clicked.\n");
            gameState->menuState = 44;
            }
        } else if (SDL_PointInRect(&mousePos, &gameState->joinButton.bounds)) {
            // Add the action for the join button here
            printf("join button clicked.\n");
            gameState->menuState = 22;
        } else if (SDL_PointInRect(&mousePos, &gameState->exitButton.bounds)) {
            // Add the action for the exit button here
            printf("exit button clicked.\n");
            gameState->menuState = 33;
        }
    }
}
