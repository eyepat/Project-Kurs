#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "model.h"
#include "view.h"


void renderField(SDL_Renderer *renderer, SDL_Texture *fieldTexture,int windowWidth, int windowHeight) {
    // Define the new size of the field
    int newWidth = windowWidth * 1; // 100% of the window width
    int newHeight = windowHeight * 1; // 100% of the window height

    // Calculate the position to center the field
    int posX = (windowWidth - newWidth) / 2;
    int posY = (windowHeight - newHeight) / 2;

    SDL_Rect fieldRect = {posX, posY, newWidth, newHeight};

    // Render the playing field within the defined rectangle
    SDL_RenderCopy(renderer, fieldTexture, NULL, &fieldRect);
}

void renderPlayers(SDL_Renderer *renderer, const GameState *gameState) {
    
   // Iterate over each player in the game state and Render the player entity
    for (int i = 0; i < gameState->numPlayers; i++)
    {
        drawDetailedCircle(renderer, gameState->players[i].x, gameState->players[i].y , gameState->players[i].radius, 3, (int *) gameState->players[i].colorData);
    }
}

void renderBall(SDL_Renderer *renderer, const GameState *gameState) {
    // Set color for the ball (using white for this example)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
    
    // Draw the ball
    int x = (int)gameState->ball.x;
    int y = (int)gameState->ball.y;
    int radius = gameState->ball.radius;
    
    for (int w = -radius; w < radius; w++) {
        for (int h = -radius; h < radius; h++) {
            if (w * w + h * h < radius * radius) {
                SDL_RenderDrawPoint(renderer, x + w, y + h);
            }
        }
    }
}

void drawDetailedCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, int outlineThickness, int colorData[4]) {
   // Outline color
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black for the outline
        for (int w = -radius - outlineThickness; w <= radius + outlineThickness; w++) {
            for (int h = -radius - outlineThickness; h <= radius + outlineThickness; h++) {
                int distanceSquared = w * w + h * h;
                if (distanceSquared >= (radius * radius) && distanceSquared <= (radius + outlineThickness) * (radius + outlineThickness)) {
                    SDL_RenderDrawPoint(renderer, centerX + w, centerY + h);
                }
            }
        }
        // Player color RGB dvs changing of this colors bettween 255 and 0 make a different collor 
        int playerR = colorData[0]; // Red 
        int playerG = colorData[1];  // Green
        int playerB = colorData[2];  // Blue 
        int playerA = colorData[3]; // Alpha component of the player's color (255 is fully opaque)
    
        // Gradient fill
        for (int w = -radius; w <= radius; w++) {
            for (int h = -radius; h <= radius; h++) {
                if ((w * w + h * h) <= (radius * radius)) {
                    int distance = sqrt(w * w + h * h);
                    int alpha = 255 - (255 * distance / radius);
                    SDL_SetRenderDrawColor(renderer, playerR, playerG, playerB, alpha); // Color for the player
                    SDL_RenderDrawPoint(renderer, centerX + w, centerY + h);
                }
            }
        }
}

// Function to render text
void renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color, int x, int y) {
    // Render text
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // Get text dimensions
    int textWidth, textHeight;
    SDL_QueryTexture(texture, NULL, NULL, &textWidth, &textHeight);

    // Position the text
    SDL_Rect textRect = {x, y, textWidth, textHeight};

    // Render the text on the screen
    SDL_RenderCopy(renderer, texture, NULL, &textRect);

    // Clean up
    SDL_DestroyTexture(texture);
}

// Render the timer text
void renderTimer(SDL_Renderer* renderer, TTF_Font* font, const Timer* timer, int windowWidth) {
    int totalSeconds = getCurrentTime(timer);
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    char text[20];
    sprintf(text, "%02d:%02d", minutes, seconds);

    SDL_Color color = {255, 255, 255};

    // Position the timer text
    int timerX = (windowWidth / 2 ) - 65; 
    int timerY = 10; 

    renderText(renderer, font, text, color, timerX, timerY);
}

// Render the score text
void renderScore(SDL_Renderer* renderer, TTF_Font* font, Score *score, int windowWidth, int windowHeight) {
    char text[20];
    sprintf(text, "%d : %d", score->team1Score, score->team2Score);  // Format: "Team1Score : Team2Score"

    SDL_Color color = {255, 255, 255};  // vit färg för poängtexten
    renderText(renderer, font, text, color, 145, 15);  // Justera positionen vid behov
}

void renderGoals(SDL_Renderer *renderer, const Field *field) {
    // Outline color, for example, black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    int outlineThickness = 2; // Adjust thickness as needed

    // Render left goal outline
    SDL_Rect outlineRectLeft = {field->goals[0].box.x - outlineThickness, field->goals[0].box.y - outlineThickness,
                                field->goals[0].box.w + outlineThickness * 2, field->goals[0].box.h + outlineThickness * 2};
    SDL_RenderDrawRect(renderer, &outlineRectLeft);

    // Render right goal outline
    SDL_Rect outlineRectRight = {field->goals[1].box.x - outlineThickness, field->goals[1].box.y - outlineThickness,
                                 field->goals[1].box.w + outlineThickness * 2, field->goals[1].box.h + outlineThickness * 2};
    SDL_RenderDrawRect(renderer, &outlineRectRight);

    // Set color for the left goal (red) with opacity
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 95); // Red color with 50% opacity

    // Render left goal
    SDL_RenderFillRect(renderer, &field->goals[0].box);

    // Set color for the right goal (blue) with opacity
    SDL_SetRenderDrawColor(renderer, 0, 0, 200, 95); // Blue color with 50% opacity

    // Render right goal
    SDL_RenderFillRect(renderer, &field->goals[1].box);

    // Reset color back to black for any future drawing
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
}

void modifyPlayerColors(int red, int blue, int green, int opacity, int playerColorData[4]){
    playerColorData[0] = red;
    playerColorData[1] = blue;
    playerColorData[2] = green;
    playerColorData[3] = opacity; //Genomskinlighet
    
}


void renderButton(SDL_Renderer* renderer, Button* button) {
    SDL_RenderCopy(renderer, button->texture, NULL, &button->bounds);
}


void drawMenu(SDL_Renderer* renderer, TTF_Font* font, MenuState* menuState, int windowWidth, int windowHeight, int *portPointer, char hostIP[20], int *isServerPointer) {
    // Define button sizes and positions
    const int buttonWidth = 200;
    const int buttonHeight = 50;
    const int buttonX = windowWidth / 2 - buttonWidth / 2;

    // Host Button
    menuState->hostButton.bounds = (SDL_Rect){buttonX, 100, buttonWidth, buttonHeight};
    
    // Join Button
    menuState->joinButton.bounds = (SDL_Rect){buttonX, 200, buttonWidth, buttonHeight};
    
    // Exit Button
    menuState->exitButton.bounds = (SDL_Rect){buttonX, 300, buttonWidth, buttonHeight};
    
    // Start Button
    menuState->startButton.bounds = (SDL_Rect){buttonX, 300, buttonWidth, buttonHeight};  // Same position as exit
    
    // Join Host Button
    menuState->joinHostButton.bounds = (SDL_Rect){buttonX, 300, buttonWidth, buttonHeight};  // Same position as exit
    
    // Online Button
    menuState->onlineButton.bounds = (SDL_Rect){buttonX, 100, buttonWidth, buttonHeight};  // Same position as host
    
    // Local Button
    menuState->localButton.bounds = (SDL_Rect){buttonX, 200, buttonWidth, buttonHeight};  // Same position as join
    
    // IP Input Button
    menuState->ipInputButton.bounds = (SDL_Rect){buttonX, 100, buttonWidth, buttonHeight};  // Same position as host

    strcpy(menuState->ip, "127.0.0.1");  // Set default IP

    //SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, menuState->menuBackground, NULL, NULL);

    // Render buttons based on menu state
    switch(menuState->menuState) {
        case 0:  // Start menu
            SDL_RenderCopy(renderer, menuState->onlineButton.texture, NULL, &menuState->onlineButton.bounds);
            SDL_RenderCopy(renderer, menuState->localButton.texture, NULL, &menuState->localButton.bounds);
            SDL_RenderCopy(renderer, menuState->exitButton.texture, NULL, &menuState->exitButton.bounds);
            break;
        case 1:  // Online menu
            SDL_RenderCopy(renderer, menuState->hostButton.texture, NULL, &menuState->hostButton.bounds);
            SDL_RenderCopy(renderer, menuState->joinButton.texture, NULL, &menuState->joinButton.bounds);
            
            break;
        case 9:  // Connect client to host
            SDL_RenderClear(renderer);
            menuState->menuState = 6;  // Proceed to next state
            break;
        case 11:  // Waiting for players 1/4
            renderTextMenu(renderer, font, "Waiting for players 1/4...", windowWidth / 2 - 150, 300, 300, 50);
            break;
        case 12:  // Waiting for players 2/4
           renderTextMenu(renderer, font, "Waiting for players 2/4...", windowWidth / 2 - 150, 300, 300, 50);
            break;
        case 13:  // Waiting for players 3/4
            renderTextMenu(renderer, font, "Waiting for players 3/4...", windowWidth / 2 - 150, 300, 300, 50);
            break;
        case 14:  // Starting game with 4/4 players
           renderTextMenu(renderer, font, "Starting game with 4/4 players...", windowWidth / 2 - 150, 300, 300, 50);
            break;
        case 15:  // Connecting to host
           renderTextMenu(renderer, font, "Connecting to host...", windowWidth / 2 - 150, 300, 300, 50);
            break;
        case 16:  // Connected to host and waiting
            renderTextMenu(renderer, font, "Connected to host and waiting", windowWidth / 2 - 150, 300, 300, 50);
            break;
        case 10:  // Host setup
            *isServerPointer = 1;
            menuState->menuState = 6;
            break;
        case 3:  // Host button clicked
            handleUserInput(renderer, font, menuState, portPointer, "Enter port",1200);
            break;
        case 8:  // Enter host port for connecting to host
            handleUserInput(renderer, font, menuState, portPointer, "Enter host port",windowWidth);
            *isServerPointer = 2;
            menuState->menuState = 9;
            break;
        case 4:  // Join client menu
            handleUserInput(renderer, font, menuState, NULL, "Enter host IP",windowWidth);
            strcpy(hostIP, menuState->userInputIp);
            menuState->menuState = 8;
            break;
    }
    
    SDL_RenderPresent(renderer);
}

// Helper function to render text
void renderTextMenu(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, int w, int h) {
    SDL_Color color = {255, 255, 255};  // White color
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstrect = {x, y, w, h};
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// Helper function to handle user input
void handleUserInput(SDL_Renderer* renderer, TTF_Font* font, MenuState* menuState, int* portPointer, const char* prompt, int windowWidth) {
    SDL_Color color = {255, 255, 255};  // White color
    renderText(renderer, font, prompt, color, windowWidth / 2 - 100, 50);

    int type = true;
    int position = 0;
    memset(menuState->userInputIp, 0, 20);

    while (type) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                if ((event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9) || event.key.keysym.sym == SDLK_PERIOD) {
                    if (position < 15) {
                        menuState->userInputIp[position] = event.key.keysym.sym;
                        position++;
                    }
                }
                if (event.key.keysym.sym == SDLK_BACKSPACE && position > 0) {
                    position--;
                    menuState->userInputIp[position] = 0;
                }
                if (event.key.keysym.sym == SDLK_RETURN) {
                    type = false;
                    if (portPointer != NULL) {
                        *portPointer = atoi(menuState->userInputIp);
                        menuState->menuState = 10;
                    }
                }
            }
        }
        
        SDL_RenderClear(renderer);  // Clear renderer before drawing
        SDL_RenderCopy(renderer, menuState->menuBackground, NULL, NULL);  // Render background
        SDL_RenderCopy(renderer, menuState->ipInputButton.texture, NULL, &menuState->ipInputButton.bounds);  // Render button
        renderText(renderer, font, prompt, color, windowWidth / 2 - 100, 50);  // Render prompt
        renderText(renderer, font, menuState->userInputIp, color, windowWidth / 2 - 50, 110);  // Render user input
        SDL_RenderPresent(renderer);  // Present renderer
    }
}

void initializeResources(SDL_Renderer* renderer, MenuState* menuState, Mix_Chunk* sounds[], int channels[]) {
    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "SDL_mixer could not initialize! Mix_Error: %s\n", Mix_GetError());
    }

    // Load sounds
    const char* soundFiles[NUM_SOUNDS] = {
        "resources/startwhistle.wav",
        // "resources/music.wav",
        "resources/ending.wav"
        // Add paths for other sounds here
    };

    for (int i = 0; i < NUM_SOUNDS; ++i) {
        sounds[i] = Mix_LoadWAV(soundFiles[i]);
        if (sounds[i] == NULL) {
            fprintf(stderr, "Failed to load sound %d! SDL_mixer Error: %s\n", i + 1, Mix_GetError());
        }
        channels[i] = -1; // Initialize channel tracking
    }

    // Initialize menu state
    menuState->menuState = 0;
    menuState->hostButton.texture = IMG_LoadTexture(renderer, "resources/host.png");
    menuState->joinButton.texture = IMG_LoadTexture(renderer, "resources/join.png");
    menuState->exitButton.texture = IMG_LoadTexture(renderer, "resources/exit.png");
    menuState->startButton.texture = IMG_LoadTexture(renderer, "resources/start.png");
    menuState->joinHostButton.texture = IMG_LoadTexture(renderer, "resources/join.png");
    menuState->onlineButton.texture = IMG_LoadTexture(renderer, "resources/online.png");
    menuState->localButton.texture = IMG_LoadTexture(renderer, "resources/local.png");
    menuState->ipInputButton.texture = IMG_LoadTexture(renderer, "resources/ipInput.png");
    menuState->backButton.texture = IMG_LoadTexture(renderer, "resources/back.png");
    menuState->menuBackground = IMG_LoadTexture(renderer, "resources/menu.jpg");
    menuState->gameBackground = IMG_LoadTexture(renderer, "resources/football-field.png");
}

void menuCleanup(MenuState* menuState, SDL_Renderer* renderer, TTF_Font* menufont, SDL_Window* window) {
    SDL_DestroyTexture(menuState->hostButton.texture);
    SDL_DestroyTexture(menuState->joinButton.texture);
    SDL_DestroyTexture(menuState->exitButton.texture);
    SDL_DestroyTexture(menuState->startButton.texture);
    SDL_DestroyTexture(menuState->joinHostButton.texture);
    SDL_DestroyTexture(menuState->onlineButton.texture);
    SDL_DestroyTexture(menuState->localButton.texture);
    SDL_DestroyTexture(menuState->ipInputButton.texture);
    SDL_DestroyTexture(menuState->backButton.texture);
    SDL_DestroyTexture(menuState->menuBackground);
    SDL_DestroyTexture(menuState->gameBackground);
    
    TTF_CloseFont(menufont);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void playSound(int soundIndex, Mix_Chunk *sounds[], int channels[]) {
   

    int channel = Mix_PlayChannel(-1, sounds[soundIndex - 1], 0);
   
    channels[soundIndex - 1] = channel; // Track the channel
}

//For longer sounds like background muscik
void stopSound(int soundIndex, int channels[]) {
    
    int channel = channels[soundIndex - 1];
    if (channel != -1) {
        Mix_HaltChannel(channel);
        channels[soundIndex - 1] = -1; // Reset channel tracking
    }
}

void renderGame(SDL_Renderer *renderer, SDL_Texture *fieldTexture, int windowWidth, int windowHeight, GameState *gameState, Field *field, TTF_Font *font) {
    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render game elements
    renderField(renderer, fieldTexture, windowWidth, windowHeight);
    renderGoals(renderer, field);
    renderPlayers(renderer, gameState);
    renderBall(renderer, gameState);
    renderScore(renderer, font, &gameState->scoreTracker, windowWidth, windowHeight);
    renderTimer(renderer, font, &gameState->gameTimer, windowWidth);

    // Present the rendered frame
    SDL_RenderPresent(renderer);
}
