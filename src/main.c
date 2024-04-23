#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "view.h"
#include "model.h"
#include "controller.h"

int main(int argc, char **argv) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL_ttf for text rendering
    if (TTF_Init() == -1) {
        printf("Error initializing SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    //Loading Font
    TTF_Font* font = TTF_OpenFont("resources/8bitOperatorPlus-Regular.ttf", 50);
    if (!font) {
    printf("Error loading font: %s\n", TTF_GetError());
    }


    // Create the main window
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0) {
        printf("Error getting display mode: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Football Game",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,displayMode.w,displayMode.h,SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (!window) {
        printf("Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
     // Hämta dimensionerna av fönstret
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    // Create the renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        printf("Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Load the football field texture
    SDL_Surface *fieldSurface = IMG_Load("resources/football_field.png");
    if (!fieldSurface) {
        printf("Error loading football field texture: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Texture *fieldTexture = SDL_CreateTextureFromSurface(renderer, fieldSurface);
    SDL_FreeSurface(fieldSurface);
    if (!fieldTexture) {
        printf("Error creating football field texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    

    // Initialize game entities and field
    Entity player, ball, player2;
    modifyPlayerColors(255, 255, 0, 255, player.colorData); //Default är gul och blå
    modifyPlayerColors(0, 0, 255, 255, player2.colorData); //Använd modifyPlayerColors för att ändra spelarnas färger dynamiskt senare
    Field field;
    initializeGame(&player, &ball, &field, &player2);

 
    //to track player movement
    Uint32 previousTime = SDL_GetTicks();
    Uint32 currentTime;
    float deltaTime;

    //timer
    Timer gameTimer;
    initializeTimer(&gameTimer, 70); // Sätter maxTime till 120 sekunder
    //Score
    Score gameScore;
    initializeScore(&gameScore);


   // Game loop variables
    bool closeWindow = false;
    bool up = false, down = false, left = false, right = false; //Player 1 movement variables
    bool up2 = false, down2 = false, left2 = false, right2 = false; //Player 2 movement variables
    float ballVelocityX = 0, ballVelocityY = 0;
    int scoreTrue = 0;
    
    // Main game loop
    while (!closeWindow) {
        currentTime = SDL_GetTicks();
        deltaTime = (currentTime - previousTime) / 1000.0f;  // Convert milliseconds to seconds
        previousTime = currentTime;
        // Handle events
        handleEvents(&closeWindow, &up, &down, &left, &right, &up2, &down2, &left2, &right2);
        

        // Update game state
        updatePlayerPosition(&player, up, down, left, right, &field,deltaTime);
        updatePlayerPosition(&player2, up2, down2, left2, right2, &field,deltaTime);
        updateBallPosition(&ball, &player, &player2, &field, &gameScore, deltaTime, &scoreTrue);
        updateBallPosition(&ball, &player2, &player, &field, &gameScore, deltaTime, &scoreTrue);
        if (scoreTrue)
        {
            resetGame(&player, &ball, &field, &player2);
            scoreTrue = 0;
        }
        

        SDL_GetWindowSize(window, &windowWidth, &windowHeight);
        updateTimer(&gameTimer);

        int teamNumber = 0; //temporary
        updateScore(&gameScore, teamNumber);//teamNumber= 1 or 2 depending on what team has scored when ball collision with goal for example

        // Render game
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        renderField(renderer, fieldTexture, windowWidth, windowHeight);
        renderGoals(renderer, &field);
        renderPlayer(renderer, &player, player.colorData);
        renderPlayer(renderer, &player2, player2.colorData); 
        renderBall(renderer, &ball);
        renderScore(renderer, font, gameScore, windowWidth, windowHeight);
        renderTimer(renderer, font, &gameTimer, windowWidth);
        SDL_RenderPresent(renderer);
        
        


        SDL_RenderPresent(renderer);

        // Delay for consistent frame rate
        SDL_Delay(1);
    }

    // Clean up
    SDL_DestroyTexture(fieldTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    TTF_CloseFont(font);

    return 0;
}
/*
int main(int argc, char* argv[]) {

    SDL_Texture* loadImage(const char* filepath, SDL_Renderer* renderer) {
    SDL_Surface* loadedSurface = IMG_Load(filepath);
    if (loadedSurface == NULL) {
        printf("Error: %s\n", IMG_GetError());
        return NULL;
    }

    SDL_Texture* newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    if (newTexture == NULL) {
        printf("Error: %s\n", SDL_GetError());
    }

    SDL_FreeSurface(loadedSurface);

    return newTexture;
    }
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);
    SDL_Window* window = SDL_CreateWindow("Football Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("path_to_your_font.ttf", 24); // replace with your font path and size

    GameState gameState;
    gameState.menuState = 1; //main menu
    gameState.hostButton.texture = IMG_LoadTexture(renderer, "resources/host.png");
    gameState.joinButton.texture = IMG_LoadTexture(renderer, "resources/join.png");
    gameState.exitButton.texture = IMG_LoadTexture(renderer, "resources/exit.png");
    gameState.menuBackground = IMG_LoadTexture(renderer, "resources/menu.png");
    gameState.gameBackground = IMG_LoadTexture(renderer, "resources/football-field.png");

    SDL_Event e;
    while (SDL_WaitEvent(&e) && e.type != SDL_QUIT) {
        handleMenuEvent(&e, &gameState);
        SDL_RenderClear(renderer);
        drawMenu(renderer, font, &gameState);
        SDL_RenderPresent(renderer);
    }

    // Cleanup...
    SDL_DestroyTexture(gameState.hostButton.texture);
    SDL_DestroyTexture(gameState.joinButton.texture);
    SDL_DestroyTexture(gameState.exitButton.texture);
    SDL_DestroyTexture(gameState.menuBackground);
    SDL_DestroyTexture(gameState.gameBackground);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0;
}*/