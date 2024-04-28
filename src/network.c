#include <stdio.h>      
#include <stdlib.h>    
#include <string.h>     
#include <SDL2/SDL_net.h> 
#include "model.h"
#include "network.h"

void initServer(IPaddress ip, GameState *gameState, TCPsocket *serverSocket, TCPsocket clientSockets[], SDLNet_SocketSet *socketSet) {
    printf("Initializing server...\n");

    // Open the server socket
    *serverSocket = SDLNet_TCP_Open(&ip);
    if (!*serverSocket) {
        fprintf(stderr, "Failed to open server socket: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);  
    }

    // Allocate and initialize the socket set
    *socketSet = SDLNet_AllocSocketSet(MAX_PLAYERS + 1);
    if (!*socketSet) {
        fprintf(stderr, "Failed to allocate socket set: %s\n", SDLNet_GetError());
        SDLNet_TCP_Close(*serverSocket);
        exit(EXIT_FAILURE);
    }

    SDLNet_TCP_AddSocket(*socketSet, *serverSocket);
    printf("Server initialized and listening...\n");

    // Accept incoming connections and update numPlayers
    int numConnectedPlayers = 1;
    while (numConnectedPlayers < MAX_PLAYERS) {
        TCPsocket newClientSocket = SDLNet_TCP_Accept(*serverSocket);
        if (newClientSocket) {
            printf("New client connected.\n");
            clientSockets[numConnectedPlayers] = newClientSocket;
            numConnectedPlayers++;
            gameState->numPlayers = numConnectedPlayers; // Update numPlayers in GameState
        }
        printf("waiting %d.\n", numConnectedPlayers);

    }
        printf("All connected.\n");

    // Increment numPlayers for the host
    gameState->numPlayers++;
}



void initClient(IPaddress ip, GameState *gameState, TCPsocket *clientSocket) {
    printf("Initializing client...\n");

    // Open the client socket
    *clientSocket = SDLNet_TCP_Open(&ip);
    if (!*clientSocket) {
        fprintf(stderr, "Failed to open client socket: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        exit(EXIT_FAILURE);  
    }

    printf("Client connected to server.\n");
}

void updateGameState(GameState *gameState, Entity *incomingPlayers) {
    for (int i = 0; i < gameState->numPlayers; i++) {
        gameState->players[i].x = incomingPlayers[i].x;
        gameState->players[i].y = incomingPlayers[i].y;
        gameState->players[i].xSpeed = incomingPlayers[i].xSpeed;
        gameState->players[i].ySpeed = incomingPlayers[i].ySpeed;
        memcpy(gameState->players[i].colorData, incomingPlayers[i].colorData, sizeof(gameState->players[i].colorData));
    }
}

void receiveDataFromClients(TCPsocket* clientSockets, SDLNet_SocketSet socketSet, GameState *gameState) {
    for (int i = 0; i < gameState->numPlayers; i++) {
        if (SDLNet_SocketReady(clientSockets[i])) {
            Entity incomingPlayers[gameState->numPlayers];
            int recvResult = SDLNet_TCP_Recv(clientSockets[i], incomingPlayers, sizeof(Entity) * gameState->numPlayers);
            if (recvResult > 0) {
                updateGameState(gameState, incomingPlayers);
            } else if (recvResult == 0) {
                printf("Client disconnected. Removing from game.\n");
                SDLNet_TCP_DelSocket(socketSet, clientSockets[i]);
                SDLNet_TCP_Close(clientSockets[i]);
                for (int j = i; j < gameState->numPlayers - 1; j++) {
                    clientSockets[j] = clientSockets[j + 1];
                }
                gameState->numPlayers--;
                i--;
            } else {
                fprintf(stderr, "Error receiving data from client %d: %s\n", i, SDLNet_GetError());
            }
        }
    }
}

void sendDataToClients(TCPsocket* clientSockets, GameState *gameState) {
    for (int i = 0; i < gameState->numPlayers; i++) {
        if (clientSockets[i] != NULL) {
            SDLNet_TCP_Send(clientSockets[i], gameState, sizeof(GameState));
        }
    }
}

void sendDataToServer(TCPsocket clientSocket, GameState *gameState) {
    SDLNet_TCP_Send(clientSocket, gameState, sizeof(GameState));
}

void receiveDataFromServer(TCPsocket clientSocket, GameState *gameState) {
    int len = SDLNet_TCP_Recv(clientSocket, gameState, sizeof(GameState));
    if (len <= 0) {
        fprintf(stderr, "Error receiving data or server disconnected: %s\n", SDLNet_GetError());
        SDLNet_TCP_Close(clientSocket);
        SDLNet_Quit();
        exit(EXIT_FAILURE);
    }
}

