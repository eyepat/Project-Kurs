#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_mixer.h>
#include "view.h"
#include "model.h"
#include "controller.h"
#include "network.h"

int main(int argc, char **argv) {

    initializeSDL();

    //Game Data
    Client clients[MAX_PLAYERS]; //HOST
    Client myClientInfo; //CLIENT
    GameState gameState; //ALL USERS


    //For menu
    int port;
    char hostIP[20];
    int isServer = 77;
    int *portPointer = &port;
    int *isServerPointer = &isServer;
    int windowWidth, windowHeight;

    SDL_Window *window = SDL_CreateWindow("Football Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    //Resource Init
    Mix_Chunk *sounds[NUM_SOUNDS];
    int channels[NUM_SOUNDS];
    MenuState menuState;
    initializeResources(renderer, &menuState, sounds, channels, window);
    TTF_Font* menufont = TTF_OpenFont("resources/8bitOperatorPlus-Regular.ttf", 24);
    TTF_Font* font = TTF_OpenFont("resources/8bitOperatorPlus-Regular.ttf", 50);
    if (!font) {
        printf("Error loading font: %s\n", TTF_GetError());
    }

    //MENU loop
    bool closeWindow = false;
    while (!closeWindow) {
        handleMenuEvent(&closeWindow, &menuState);
        SDL_RenderClear(renderer);
        drawMenu(renderer, menufont, &menuState, windowWidth, windowHeight, portPointer, hostIP, isServerPointer);
        SDL_Delay(11);

        if (menuState.menuState == 6 || menuState.menuState == 2) {
            closeWindow = true;
            if (menuState.menuState == 2) {
                isServer = 0;
            }
        }
    }

    //Network
    SDLNet_SocketSet socketSet;
    IPaddress ip;

    switch (isServer) {
        case 1: //SERVER
            printf("Port is : %d \n", port);

            if (SDLNet_ResolveHost(&ip, NULL, port) != 0) {
                fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
                SDLNet_Quit();
                return -1;
            }
            initServer(ip, &gameState, clients, &socketSet, renderer, menufont, windowWidth, &menuState, windowHeight, portPointer, hostIP, isServerPointer, &closeWindow);
            break;

        case 2: //CLIENT
            printf("Host IP entered: %s \n", hostIP);
            printf("Host port entered: %d\n", port);

            if (SDLNet_ResolveHost(&ip, hostIP, port) != 0) {
                fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
                SDLNet_Quit();
                return -1;
            }
            initClient(ip, &myClientInfo, &gameState, renderer, menufont, windowWidth, &menuState, windowHeight, portPointer, hostIP, isServerPointer, &closeWindow);
            break;

        case 0: // LOCAL
            gameState.numPlayers = 2;
            clients[0].clientID = 0; //WASD
            clients[1].clientID = 1; //ARROWS
            printf("Initializing local game\n");
            break;

        default:
            break;
    }
    menuCleanup(&menuState, renderer, menufont, window);

    //For game
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

    int frameDelay = 1000 / FPS; // Milliseconds per frame
    int scoreTrue = 0; 
    Field field;
    Uint32 previousTime = SDL_GetTicks();
    Uint32 currentTime;
    float deltaTime;
    closeWindow = false;
    //if exit button in menu is pressed
    if (menuState.menuState == 77) {
        closeWindow = true;
    }

    initializeGame(&gameState, &field, clients);

    playSound(1, sounds, channels);
    playSound(3, sounds, channels);

    // Main game loop
    while (!closeWindow && !gameState.isGameOver) 
    {
        currentTime = SDL_GetTicks();
        deltaTime = (currentTime - previousTime) / 1000.0f;
        previousTime = currentTime;
        //CONTROLLER
        handleEvents(&closeWindow, clients, &gameState, isServer, &myClientInfo);
      
        if (isServer == 1 || isServer == 0) { // HOST and LOCAL

            if (isServer == 1) { // Additional actions for HOST
                receiveDataFromClients(clients, socketSet, &gameState);
            }
            //MODEL
            updatePlayerPosition(&gameState, clients, &field, deltaTime);
            updateTimer(&gameState.gameTimer, &gameState); 
            updateBallPosition(&gameState.ball, &gameState, &field, &gameState.scoreTracker, deltaTime, &scoreTrue);
            if (scoreTrue) {
                playSound(1, sounds, channels);
                resetGameAfterGoal(&gameState, &gameState.ball, &field);
                scoreTrue = 0;
            }
            if (isServer == 1) { // Additional actions for HOST
                sendDataToClients(clients, &gameState);
            }
        } else if (isServer == 2) { // CLIENT
            sendDataToServer(&myClientInfo, &gameState);
            receiveDataFromServer(&myClientInfo, &gameState);
        }

        //VIEW
        renderGame(renderer, fieldTexture, windowWidth, windowHeight, &gameState, &field, font);

        // Delay to maintain 60 FPS
        currentTime = SDL_GetTicks() - currentTime;
        if (frameDelay > currentTime) {
            SDL_Delay(frameDelay - currentTime);
        }
    }
    stopSound(3, channels);
    if(gameState.isGameOver && menuState.menuState!=77)
    {
        playSound(2, sounds, channels);
        handleGameOver(&closeWindow, &gameState, renderer, font, &field, isServer, clients, socketSet);
    }
   
    cleanup(&gameState, fieldTexture, renderer, window, font, clients, &myClientInfo, socketSet);
    return 0;
}