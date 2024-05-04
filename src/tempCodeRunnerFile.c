#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>
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
        exit(EXIT_FAILURE);
    }


    bool isServer;
    GameState gameState;
    gameState.numPlayers = 1;


     TCPsocket clientSocket;  // Client's socket
     TCPsocket serverSockets[MAX_PLAYERS];  // Array of client sockets for the server
     SDLNet_SocketSet socketSet;

     int choice, port;
     IPaddress ip;
     char hostIP[20];  // Buffer for storing IP address

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
            getchar(); // Clear the newline character from input buffer

            // Set up the server to listen on all interfaces
           if (SDLNet_ResolveHost(&ip, NULL, port) != 0) {
                 fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
                 SDLNet_Quit();
                return -1;
             }

            initServer(ip, &gameState, &serverSockets[0], serverSockets, &socketSet);
              printf("Waiting for clients continue? 1/yes \n ");
             int yes;
              scanf("%d", &yes);

             break;

        case 2:  // Client
            isServer = false;
             printf("Enter server IP address to connect (e.g., 127.0.0.1): ");
            fgets(hostIP, sizeof(hostIP), stdin);
             strtok(hostIP, "\n");  // Remove newline character

            printf("Enter server port number: ");
            scanf("%d", &port);
             getchar(); // Clear the newline character from input buffer

            if (SDLNet_ResolveHost(&ip, hostIP, port) != 0) {
                 fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
                 SDLNet_Quit();
                 return -1;
             }
             initClient(ip, &gameState, &clientSocket);
             break;

        default:
             printf("Invalid choice! Please enter a valid option.\n");
             return -1; // Exit if no valid choice
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

        // Handle events
        handleEvents(&closeWindow, flags, &gameState);

         if (isServer) {
             // Server operations
             receiveDataFromClients(serverSockets, socketSet, &gameState);
              printf("server recive.\n");
 
             sendDataToClients(serverSockets, &gameState);  
             printf("server send.\n");

         } else {
            // Client operations
             sendDataToServer(clientSocket, &gameState); 
             printf("server send.\n");

             receiveDataFromServer(clientSocket, &gameState); //dark screen when this one is on and crashed when server started the gameloop
         }   
         printf("Servers or client.\n");// här

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

     if (isServer) {
     for (int i = 0; i < gameState.numPlayers; i++) {
         if (serverSockets[i]) {
             SDLNet_TCP_Close(serverSockets[i]);
         }
     }
     } else {
         SDLNet_TCP_Close(clientSocket);
     }

    SDLNet_Quit();
    SDL_Quit();
    TTF_CloseFont(font);

    return 0;
}
