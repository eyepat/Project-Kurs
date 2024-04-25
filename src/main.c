#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
// #include <SDL2/SDL_net.h>
#include "view.h"
#include "model.h"
#include "controller.h"

int main(int argc, char **argv) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    // // Initialize SDL_net
    // if (SDLNet_Init() == -1) {
    //     printf("Error: %s\n", SDL_GetError());
    //     return 1;    
    // }

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
    
    //initialize game
    int numPlayers = 2; // Initialize numPlayers with the actual number of players; 
    Entity players[4]; // number of players you want to initialize
    Entity ball;
    Field field;
    MovementFlags flags[4] = {0}; // Using {0} initializes all fields to false
    initializeGame(players, numPlayers, &ball, &field);

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
    float ballVelocityX = 0, ballVelocityY = 0;
    int scoreTrue = 0;
    
    // Main game loop
    while (!closeWindow) {
        currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - previousTime) / 1000.0f;  // Convert milliseconds to seconds
        previousTime = currentTime;
        // Client function: Send player array data to host temp example
        // sendPlayerArrayToHost(players, numPlayers);

        // Host function: Receive player array data from clients and update game state temp example
        // receivePlayerArrayFromClientsAndUpdateGameState(players, numPlayers);

        // Handle events
        handleEvents(&closeWindow, flags, numPlayers);
        updatePlayerPosition(players, flags, numPlayers, &field, deltaTime);

        updateBallPosition(&ball, players, numPlayers, &field, &gameScore, deltaTime, &scoreTrue);
        
        if (scoreTrue) {
            resetGame(players, &ball, &field, numPlayers);
            scoreTrue = 0;
        }

        SDL_GetWindowSize(window, &windowWidth, &windowHeight);
        updateTimer(&gameTimer);

        // Render game
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        renderField(renderer, fieldTexture, windowWidth, windowHeight);
        renderGoals(renderer, &field);
        renderPlayers(renderer, players, numPlayers);
        renderBall(renderer, &ball);
        renderScore(renderer, font, gameScore, windowWidth, windowHeight);
        renderTimer(renderer, font, &gameTimer, windowWidth);
        SDL_RenderPresent(renderer);

        // Delay for consistent frame rate
        SDL_Delay(1);
    }

    // Clean up
    SDL_DestroyTexture(fieldTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    // SDLNet_Quit();
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
    SDL_Window* window = SDL_CreateWindow("Football Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_FULLSCREEN_DESKTOP);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("path_to_your_font.ttf", 24); // replace with your font path and size

    GameState gameState;
    gameState.menuState = 1; //main menu
    gameState.hostButton.texture = IMG_LoadTexture(renderer, "resources/host.png");
    gameState.joinButton.texture = IMG_LoadTexture(renderer, "resources/join.png");
    gameState.exitButton.texture = IMG_LoadTexture(renderer, "resources/exit.png");
    gameState.startButton.texture = IMG_LoadTexture(renderer, "resources/start.png");
    gameState.menuBackground = IMG_LoadTexture(renderer, "resources/menu.png");
    gameState.gameBackground = IMG_LoadTexture(renderer, "resources/football-field.png");
    
    gameState.hostButton.bounds.x = 100; // 100 pixels from the left edge of the window
    gameState.hostButton.bounds.y = 100; // 100 pixels from the top edge of the window
    gameState.hostButton.bounds.w = 200; // Button width is 200 pixels
    gameState.hostButton.bounds.h = 50;  // Button height is 50 pixels
    
    gameState.joinButton.bounds.x = 100; // Same x as hostButton for alignment
    gameState.joinButton.bounds.y = 200; // 200 pixels from the top, so it's below hostButton
    gameState.joinButton.bounds.w = 200; // Same width as hostButton
    gameState.joinButton.bounds.h = 50;  // Same height as hostButton
    
    gameState.exitButton.bounds.x = 100; // Same x as other buttons for alignment
    gameState.exitButton.bounds.y = 300; // Below the other buttons
    gameState.exitButton.bounds.w = 200; // Same width as other buttons
    gameState.exitButton.bounds.h = 50;  // Same height as other buttons
    
    gameState.startButton.bounds.x = 100; // Same x as other buttons for alignment
    gameState.startButton.bounds.y = 300; // Below the other buttons
    gameState.startButton.bounds.w = 200; // Same width as other buttons
    gameState.startButton.bounds.h = 50;  // Same height as other buttons

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
    SDL_DestroyTexture(gameState.startButton.texture);
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