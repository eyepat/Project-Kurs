#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "SDL_image.h"
#include <SDL_ttf.h>
#include <SDL_net.h>
#include "view.h"
#include "model.h"
#include "controller.h"
#include "network.h"

int main(int argc, char **argv) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL_net
    if (SDLNet_Init() != 0) {
        fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
        // exit(EXIT_FAILURE);
    }

   
    GameState gameState;
    gameState.numPlayers = MAX_PLAYERS;

    TCPsocket serverSocket, clientSocket;
    TCPsocket clientSockets[MAX_PLAYERS];
    SDLNet_SocketSet socketSet;
    IPaddress ip;

    int choice, port;
    char hostIP[20];  
    bool isServer;


    // Basic join menu for terminal
    printf("\nChoose an option:\n");
    printf("1. Host a server\n");
    printf("2. Connect as a client\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar();  

    switch (choice) {
        case 1:  // Server
            isServer = true;
            printf("Enter port number: ");
            scanf("%d", &port);
            getchar(); 

            // Set up the server to listen on all interfaces
            if (SDLNet_ResolveHost(&ip, NULL, port) != 0) {
                fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
                SDLNet_Quit();
                return -1;
            }

            initServer(ip, &gameState, &serverSocket, clientSockets, &socketSet);
        
            break;

        case 2:  // clients
            isServer = false;
            printf("Enter server IP address to connect (e.g., 127.0.0.1): ");
            fgets(hostIP, sizeof(hostIP), stdin);
            strtok(hostIP, "\n"); 

            printf("Enter server port number: ");
            scanf("%d", &port);
            getchar();

            if (SDLNet_ResolveHost(&ip, hostIP, port) != 0) {
                fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
                SDLNet_Quit();
                return -1;
            }
            initClient(ip, &gameState, &clientSocket);
            break;

        default:
            printf("Invalid choice! Please enter a valid option.\n");
            return -1; 
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
    
    // //initialize game
    Entity ball;
    Field field;
    MovementFlags flags[4] = {0};   
    initializeGame(&gameState, &ball, &field);

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


        if (isServer) {
            // Server operations
            // acceptNewOrReconnectingClients(serverSocket, clientSockets, socketSet, &gameState);// for re connecting disconnected playesr 
            receiveDataFromClients(clientSockets, socketSet, &gameState); 
            sendDataToClients(clientSockets, &gameState);  
        } else {
            // Client operations
            sendDataToServer(clientSocket, &gameState); 
            receiveDataFromServer(clientSocket, &gameState); 
        }   

        // Handle events
        handleEvents(&closeWindow, flags, &gameState);

        updatePlayerPosition(&gameState, flags, &field, deltaTime);
        updateBallPosition(&ball, &gameState, &field, &gameScore, deltaTime, &scoreTrue);
        
        if (scoreTrue) {
            resetGame(&gameState, &ball, &field);
            scoreTrue = 0;
        }

        SDL_GetWindowSize(window, &windowWidth, &windowHeight);
        updateTimer(&gameTimer);

        // Render game
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        renderField(renderer, fieldTexture, windowWidth, windowHeight);
        renderGoals(renderer, &field);
        renderPlayers(renderer, &gameState);
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

    // Clean up client sockets array
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (clientSockets[i] != NULL) {
            SDLNet_TCP_Close(clientSockets[i]);
            clientSockets[i] = NULL;
        }
    }

    // // Clean up server socket
    SDLNet_TCP_Close(serverSocket);
    // serverSocket = NULL;

    // Clean up SDLNet socket set
    SDLNet_FreeSocketSet(socketSet);
    // socketSet = NULL;

    SDLNet_Quit();
    TTF_CloseFont(font);
    SDL_Quit();

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

    MenuState menuState;
    menuState.menuState = 0; 
    menuState.hostButton.texture = IMG_LoadTexture(renderer, "resources/host.png");
    menuState.joinButton.texture = IMG_LoadTexture(renderer, "resources/join.png");
    menuState.exitButton.texture = IMG_LoadTexture(renderer, "resources/exit.png");
    menuState.startButton.texture = IMG_LoadTexture(renderer, "resources/start.png");
    menuState.joinHostButton.texture = IMG_LoadTexture(renderer, "resources/join.png");
    menuState.menuBackground = IMG_LoadTexture(renderer, "resources/menu.png");
    menuState.gameBackground = IMG_LoadTexture(renderer, "resources/football-field.png");

    bool closeWindow = false;
    while (!closeWindow) {
        handleMenuEvent(&closeWindow, &menuState);
        SDL_RenderClear(renderer);
        drawMenu(renderer, font, &menuState);
        SDL_RenderPresent(renderer);

        if (menuState.menuState == 33) //exit button clicked
        {
            closeWindow=true;
        }
    }

    // Cleanup...
    SDL_DestroyTexture(menuState.hostButton.texture);
    SDL_DestroyTexture(menuState.joinButton.texture);
    SDL_DestroyTexture(menuState.exitButton.texture);
    SDL_DestroyTexture(menuState.startButton.texture);
    SDL_DestroyTexture(menuState.joinHostButton.texture);
    SDL_DestroyTexture(menuState.menuBackground);
    SDL_DestroyTexture(menuState.gameBackground);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0;
}*/