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
    }

    Client clients[MAX_PLAYERS];
    Client myClientInfo;

    for (int i = 0; i < MAX_PLAYERS; i++) {
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
    int isServer = 77;
    int *portPointer = &port;
    int *isServerPointer = &isServer;

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);
    SDL_Window *window = SDL_CreateWindow("Football Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    TTF_Font* menufont = TTF_OpenFont("resources/8bitOperatorPlus-Regular.ttf", 24);

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
    menuState.backButton.texture = IMG_LoadTexture(renderer, "resources/back.png");
    menuState.menuBackground = IMG_LoadTexture(renderer, "resources/menu.jpg");
    menuState.gameBackground = IMG_LoadTexture(renderer, "resources/football-field.png");

    bool closeWindow = false;
    while (!closeWindow) {
        handleMenuEvent(&closeWindow, &menuState);
        SDL_RenderClear(renderer);
        drawMenu(renderer, menufont, &menuState, windowWidth, windowHeight, portPointer, hostIP, isServerPointer);
        SDL_Delay(11);

        if (menuState.menuState == 6 || menuState.menuState == 2) {
            SDL_RenderClear(renderer);
            closeWindow = true;
            if (menuState.menuState == 2) {
                isServer = 0;
            }
        }
    }

    switch (isServer) {
        case 1: 
            isServer = 1;
            printf("Port is : %d \n", port);

            if (SDLNet_ResolveHost(&ip, NULL, port) != 0) {
                fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
                SDLNet_Quit();
                return -1;
            }
            printf("Initializing server\n");
            initServer(ip, &gameState, clients, &socketSet, renderer, menufont, windowWidth, &menuState, windowHeight, portPointer, hostIP, isServerPointer, &closeWindow);
            break;

        case 2:
            isServer = 2;
            printf("Host IP entered: %s \n", hostIP);
            printf("Host port entered: %d\n", port);

            if (SDLNet_ResolveHost(&ip, hostIP, port) != 0) {
                fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
                SDLNet_Quit();
                return -1;
            }
            initClient(ip, &myClientInfo, &gameState, renderer, menufont, windowWidth, &menuState, windowHeight, portPointer, hostIP, isServerPointer, &closeWindow);
        default:
            break;
    }

    SDL_DestroyTexture(menuState.hostButton.texture);
    SDL_DestroyTexture(menuState.joinButton.texture);
    SDL_DestroyTexture(menuState.exitButton.texture);
    SDL_DestroyTexture(menuState.startButton.texture);
    SDL_DestroyTexture(menuState.joinHostButton.texture);
    SDL_DestroyTexture(menuState.onlineButton.texture);
    SDL_DestroyTexture(menuState.localButton.texture);
    SDL_DestroyTexture(menuState.ipInputButton.texture);
    SDL_DestroyTexture(menuState.menuBackground);
    SDL_DestroyTexture(menuState.gameBackground);
    TTF_CloseFont(menufont);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    if (TTF_Init() == -1) {
        printf("Error initializing SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("resources/8bitOperatorPlus-Regular.ttf", 50);
    if (!font) {
        printf("Error loading font: %s\n", TTF_GetError());
    }

    window = SDL_CreateWindow("Football Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    if (!window) {
        printf("Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        printf("Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

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

    while (isServer == 0 || isServer == 1 || isServer == 2) { // Main game loop for restart functionality
        gameState.ball.x = 0;
        gameState.ball.y = 0;
        Field field;

        if (isServer != 0) {
            initializeGame(&gameState, &field);
        }

        if (isServer == 0) {
            gameState.numPlayers = 2;
            initializeGame(&gameState, &field);
        }
        MovementFlags localMovement[2];

        for (int i = 0; i < 2; i++)
        {
            localMovement[i].right = 0;
            localMovement[i].up = 0;
            localMovement[i].left = 0;
            localMovement[i].down = 0;
        }
        

        Uint32 previousTime = SDL_GetTicks();
        Uint32 currentTime;
        float deltaTime;

        Timer timer;    
        initializeTimer(&gameState.gameTimer, 120);

        initializeScore(&gameState.scoreTracker);

        closeWindow = false;
        int scoreTrue = 0;

        while (!closeWindow && !gameState.isGameOver) {
            currentTime = SDL_GetTicks();
            deltaTime = (currentTime - previousTime) / 1000.0f;
            previousTime = currentTime;

            if (isServer == 1) {
                handleHostEvents(&closeWindow, clients, &gameState);
                receiveDataFromClients(clients, socketSet, &gameState);
                updatePlayerPosition(&gameState, clients, &field, deltaTime);
                updateBallPosition(&gameState.ball, &gameState, &field, &gameState.scoreTracker, deltaTime, &scoreTrue);
                updateTimer(&gameState.gameTimer, &gameState);

                // If a goal is scored, reset the ball and players' positions
                if (scoreTrue) {
                    resetGameAfterGoal(&gameState, &gameState.ball, &field);
                    scoreTrue = 0;
                }

                sendDataToClients(clients, &gameState);
            } else if (isServer == 2) {
                handleClientEvents(&closeWindow, &myClientInfo);
                sendDataToServer(&myClientInfo, &gameState);
                receiveDataFromServer(&myClientInfo, &gameState);
            } else if (isServer == 0) {
                localControls(&closeWindow, &gameState, localMovement);
                updatePlayerPositionLocal(&gameState, &field, deltaTime, localMovement);
                updateTimer(&gameState.gameTimer, &gameState);
                updateBallPosition(&gameState.ball, &gameState, &field, &gameState.scoreTracker, deltaTime, &scoreTrue);

                // If a goal is scored, reset the ball and players' positions
                if (scoreTrue) {
                    resetGameAfterGoal(&gameState, &gameState.ball, &field);
                    scoreTrue = 0;
                }
            }

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
            SDL_Delay(5);
        }

        /*if (gameState.isGameOver) {
            handleGameOver(&closeWindow, &gameState, renderer, font, &field, isServer, clients, socketSet);
        }

        if (closeWindow) {
            break;
        }*/
    }

    cleanup(fieldTexture, renderer, window, font, clients, &myClientInfo, socketSet, serverSocket);

    return 0;
}