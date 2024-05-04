#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "SDL2/SDL_image.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>
#include "view.h"
#include "model.h"
#include "controller.h"
#include "network.h"

__attribute__((constructor))
void initialize_console() {
    setbuf(stdout, NULL);
}

int main(int argc, char **argv) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    if (SDLNet_Init() != 0) {
        fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
        SDL_Quit();
        return 1;
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

    printf("\nChoose an option:\n");
    printf("1. Host a server\n");
    printf("2. Connect as a client\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar();

    switch (choice) {
        case 1:
            isServer = true;
            printf("Enter port number: ");
            scanf("%d", &port);
            getchar();
            if (SDLNet_ResolveHost(&ip, NULL, port) != 0) {
                fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
                SDLNet_Quit();
                SDL_Quit();
                return -1;
            }
            initServer(ip, &gameState, &serverSocket, clientSockets, &socketSet);
            break;
        case 2:
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
                SDL_Quit();
                return -1;
            }
            initClient(ip, &gameState, &clientSocket);
            break;
        default:
            printf("Invalid choice! Please enter a valid option.\n");
            return -1;
    }

    if (TTF_Init() == -1) {
        printf("Error initializing SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("resources/8bitOperatorPlus-Regular.ttf", 50);
    if (!font) {
        printf("Error loading font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Football Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (!window) {
        printf("Error creating window: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    // Get the window width and height
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Surface *fieldSurface = IMG_Load("resources/football_field.png");
    if (!fieldSurface) {
        printf("Error loading football field texture: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    SDL_Texture *fieldTexture = SDL_CreateTextureFromSurface(renderer, fieldSurface);
    SDL_FreeSurface(fieldSurface);

    Entity ball;
    Field field;
    initializeGame(&gameState, &ball, &field);
    MovementFlags flags[MAX_PLAYERS] = {0};
    Uint32 previousTime = SDL_GetTicks();
    bool closeWindow = false;

    while (!closeWindow) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - previousTime) / 1000.0f;
        previousTime = currentTime;

        if (isServer) {
            receiveDataFromClients(clientSockets, socketSet, &gameState);
            sendDataToClients(clientSockets, &gameState);
        } else {
            sendDataToServer(clientSocket, &gameState);
            receiveDataFromServer(clientSocket, &gameState);
           
        }

        handleEvents(&closeWindow, flags, &gameState);
        updatePlayerPosition(&gameState, flags, &field, deltaTime);
        int scoreFlag = 0;
        updateBallPosition(&ball, &gameState, &field, deltaTime, &scoreFlag);
        if (scoreFlag) {
            resetGame(&gameState, &ball, &field);
        }

       updateTimer(&gameState);


        SDL_RenderClear(renderer);
        renderField(renderer, fieldTexture, windowWidth, windowHeight);
        renderGoals(renderer, &field);
        renderPlayers(renderer, &gameState);
        renderBall(renderer, &ball);
       renderScore(renderer, font, gameState.score, windowWidth, windowHeight);
       renderTimer(renderer, font, &gameState.timer, windowWidth);
        SDL_RenderPresent(renderer);

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