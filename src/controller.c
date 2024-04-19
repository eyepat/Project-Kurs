#include "controller.h"

void handleEvents(bool *closeWindow, bool *up, bool *down, bool *left, bool *right, bool *up2, bool *down2, bool *left2, bool *right2) {
    // Handle user input and events to control game state
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                *closeWindow = true;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_W:
                        *up = true;
                        break;
                    case SDL_SCANCODE_UP:
                        *up2 = true;
                        break;    
                    case SDL_SCANCODE_S:
                        *down = true;
                        break;
                    case SDL_SCANCODE_DOWN:
                        *down2 = true;
                        break;
                    case SDL_SCANCODE_A:
                        *left = true;
                        break;
                    case SDL_SCANCODE_LEFT:
                        *left2 = true;
                        break;
                    case SDL_SCANCODE_D:
                        *right = true;
                        break;
                    case SDL_SCANCODE_RIGHT:
                        *right2 = true;
                        break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_W:
                        *up = false;
                        break;
                    case SDL_SCANCODE_UP:
                        *up2 = false;
                        break;    
                    case SDL_SCANCODE_S:
                        *down = false;
                        break;
                    case SDL_SCANCODE_DOWN:
                        *down2 = false;
                        break;
                    case SDL_SCANCODE_A:
                        *left = false;
                        break;
                    case SDL_SCANCODE_LEFT:
                        *left2 = false;
                        break;
                    case SDL_SCANCODE_D:
                        *right = false;
                        break;
                    case SDL_SCANCODE_RIGHT:
                        *right2 = false;
                        break;
                }
                break;
        }
    }
}
