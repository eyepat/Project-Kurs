#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "SDL2/SDL_image.h"
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
        // exit(EXIT_FAILURE);
    }

   

    Client clients[MAX_PLAYERS];  // Array to manage client connections
    Client myClientInfo;

    // clients[0].isActive = 1; 
    // clients[0].clientID = 0;  


    for (int i = 0; i < MAX_PLAYERS; i++) {
        // Initialize the MovementFlags struct for each client
        clients[i].flags.up = false;
        clients[i].flags.down = false;
        clients[i].flags.left = false;
        clients[i].flags.right = false;

    }


    GameState gameState;
    SDLNet_SocketSet socketSet;
    
    TCPsocket serverSocket;
    IPaddress ip;

    int choice, port;
    char hostIP[20];
    bool isServer;

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

            initServer(ip, &gameState, clients, &socketSet);
            // clients[0].clientID = 0;  
            // clients[0].isActive = 1; 
            break;

        case 2:  // Client
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

            initClient(ip, &myClientInfo, &gameState);
            break;

        default:
            printf("Invalid choice! Please enter a valid option.\n");
            return -1;
    }

    printf("after netwokr inits.\n");

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

    SDL_Window *window = SDL_CreateWindow(
        "Football Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0);

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
    gameState.ball.x = 0;
    gameState.ball.y = 0;
    Field field;
    
 
    initializeGame(&gameState, &field);


    //to track player movement
    Uint32 previousTime = SDL_GetTicks();
    Uint32 currentTime;
    float deltaTime;

    //timer
    Timer timer;
    initializeTimer(&gameState.gameTimer, 45); // Sätter maxTime till 120 sekunder
    //Score
    initializeScore(&gameState.scoreTracker);


    // Game loop variables
    bool closeWindow = false;
    float ballVelocityX = 0, ballVelocityY = 0;
    int scoreTrue = 0;
    
    // Main game loop
    while (!closeWindow && !gameState.isGameOver) {
        currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - previousTime) / 1000.0f;  // Convert milliseconds to seconds
        previousTime = currentTime;

        if (isServer) {
            // Server operations        
            handleHostEvents(&closeWindow, clients, &gameState);
            receiveDataFromClients(clients, socketSet, &gameState); 
            updatePlayerPosition(&gameState, clients, &field, deltaTime);
            updateBallPosition(&gameState.ball, &gameState, &field, &gameState.scoreTracker, deltaTime, &scoreTrue);
             updateTimer(&gameState.gameTimer, &gameState);
            
            if (scoreTrue) {
                resetGame(&gameState, &gameState.ball, &field);
                scoreTrue = 0;
            }

            sendDataToClients(clients, &gameState);  //funkar helt

        } else if (!isServer) {            
            // Client operations
            handleClientEvents(&closeWindow, &myClientInfo);// när borta kan it controll 
            sendDataToServer(&myClientInfo, &gameState); 
            receiveDataFromServer(&myClientInfo, &gameState); //funkar

        }   

        // Render game
        SDL_GetWindowSize(window, &windowWidth, &windowHeight);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        renderField(renderer, fieldTexture, windowWidth, windowHeight);
        renderGoals(renderer, &field);
        renderPlayers(renderer, &gameState);
        renderBall(renderer, &gameState.ball);
        renderScore(renderer, font, &gameState.scoreTracker, windowWidth, windowHeight);
        renderTimer(renderer, font, &gameState.gameTimer, windowWidth);
        SDL_RenderPresent(renderer);
        
        // Delay for consistent frame rate
        SDL_Delay(1);
    }
    // Display winner if game is over
    if (gameState.isGameOver) {
        renderWinner(renderer, font, &gameState.scoreTracker);  // Display winning team
        SDL_RenderPresent(renderer);
        SDL_Delay(10000);  // Keep the window with the result for 5 seconds
    }

    // Clean up
    SDL_DestroyTexture(fieldTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();

    // Clean up client sockets array and myClientInfo socket
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (clients[i].socket != NULL) {
            SDLNet_TCP_Close(clients[i].socket);
            clients[i].socket = NULL;
        }
    }
    if (myClientInfo.socket != NULL) {
        SDLNet_TCP_Close(myClientInfo.socket);
        myClientInfo.socket = NULL;
    }

    // Clean up SDLNet socket set
    SDLNet_FreeSocketSet(socketSet);
    socketSet = NULL;

    // Clean up server socket
    SDLNet_TCP_Close(serverSocket);
    serverSocket = NULL;


    SDLNet_Quit();
    TTF_CloseFont(font);
    SDL_Quit();

    return 0;
}





/*
int main(int argc, char* argv[]) {

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);
    SDL_Window* window = SDL_CreateWindow("Football Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_FULLSCREEN_DESKTOP);
    
    // Hämta dimensionerna av fönstret
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* menufont = TTF_OpenFont("resources/8bitOperatorPlus-Regular.ttf", 24); // replace with your font path and size
    MenuState menuState;
    menuState.menuState = 0; 
    menuState.hostButton.texture = IMG_LoadTexture(renderer, "resources/host.png");
    menuState.joinButton.texture = IMG_LoadTexture(renderer, "resources/join.png");
    menuState.exitButton.texture = IMG_LoadTexture(renderer, "resources/exit.png");
    menuState.startButton.texture = IMG_LoadTexture(renderer, "resources/start.png");
    menuState.joinHostButton.texture = IMG_LoadTexture(renderer, "resources/join.png");
    menuState.onlineButton.texture = IMG_LoadTexture(renderer, "resources/online.png");
    menuState.localButton.texture = IMG_LoadTexture(renderer, "resources/local.png");
    menuState.ipInputButton.texture = IMG_LoadTexture(renderer, "resources/ipInput.png");
    //menuState.backButton.texture = IMG_LoadTexture(renderer, "resources/back.png");
    menuState.menuBackground = IMG_LoadTexture(renderer, "resources/menu.png");
    menuState.gameBackground = IMG_LoadTexture(renderer, "resources/football-field.png");

    bool closeWindow = false;
    while (!closeWindow) {
        handleMenuEvent(&closeWindow, &menuState);
        SDL_RenderClear(renderer);
        drawMenu(renderer, menufont, &menuState, windowWidth, windowHeight);
        SDL_RenderPresent(renderer);
        //menuState->menuState = 0;//start menu, choose to play online or local
        //menuState->menuState = 1;//online menu
        //menuState->menuState = 2;//start local two player game
        //menuState->menuState = 3;//host menu
        //menuState->menuState = 4;//join menu
        //menuState->menuState = 5;//exit game
        //menuState->menuState = 6;//start game
        //menuState->menuState = 7;// enter host ip and join host

        if (menuState.menuState == 6) {//host clicked start button
            //start online game
        }
        if (menuState.menuState == 7) {//client clicked join host
            //join host game
        }
        if (menuState.menuState == 2) {//local butto clicked
            //start local game
        }
        if (menuState.menuState == 5) {//exit button clicked
            closeWindow=true;
        }
    }
     // Cleanup...
    SDL_DestroyTexture(menuState.hostButton.texture);
    SDL_DestroyTexture(menuState.joinButton.texture);
    SDL_DestroyTexture(menuState.exitButton.texture);
    SDL_DestroyTexture(menuState.startButton.texture);
    SDL_DestroyTexture(menuState.joinHostButton.texture);
    SDL_DestroyTexture(menuState.onlineButton.texture);
    SDL_DestroyTexture(menuState.localButton.texture);
    SDL_DestroyTexture(menuState.ipInputButton.texture);
    //SDL_DestroyTexture(menuState.backButton.texture);
    SDL_DestroyTexture(menuState.menuBackground);
    SDL_DestroyTexture(menuState.gameBackground);
    TTF_CloseFont(menufont);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0;

}*/

