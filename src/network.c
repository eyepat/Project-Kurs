#define MAX_PLAYERS 4
#include <stdio.h>      
#include <stdlib.h>    
#include <string.h>     
#include <SDL2/SDL_net.h> 
#include "network.h"
#include "model.h"


void initServer(IPaddress ip, Entity *players, int *numPlayers, TCPsocket *serverSocket, TCPsocket clientSockets[], SDLNet_SocketSet *socketSet) {
    printf("Initializing server...\n");

    // Open the server socket
    *serverSocket = SDLNet_TCP_Open(&ip);
    if (!*serverSocket) {
        fprintf(stderr, "Failed to open server socket: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);  // Or handle more gracefully
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
}

void initClient(IPaddress ip, Entity *players, int numPlayers, TCPsocket *clientSocket) {
    printf("Initializing client...\n");

    // Open the client socket
    *clientSocket = SDLNet_TCP_Open(&ip);
    if (!*clientSocket) {
        fprintf(stderr, "Failed to open client socket: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        exit(EXIT_FAILURE);  // Or handle more gracefully
    }

    printf("Client connected to server...\n");
}


void updateGameState(Entity* players, int numPlayers, Entity* incomingPlayers) {
    for (int i = 0; i < numPlayers; i++) {
        players[i].x = incomingPlayers[i].x;
        players[i].y = incomingPlayers[i].y;
        players[i].xSpeed = incomingPlayers[i].xSpeed;
        players[i].ySpeed = incomingPlayers[i].ySpeed;
        memcpy(players[i].colorData, incomingPlayers[i].colorData, sizeof(players[i].colorData));
    }
}


void receiveDataFromClients(TCPsocket* clientSockets, SDLNet_SocketSet socketSet, Entity* players, int* numPlayers) {
    for (int i = 0; i < *numPlayers; i++) {
        if (SDLNet_SocketReady(clientSockets[i])) {
            Entity incomingPlayers[*numPlayers]; // Use VLA based on the number of players
            int recvResult = SDLNet_TCP_Recv(clientSockets[i], incomingPlayers, sizeof(Entity) * (*numPlayers));
            if (recvResult > 0) {
                updateGameState(players, *numPlayers, incomingPlayers);
            } else if (recvResult == 0) {
                printf("Client disconnected. Removing from game.\n");
                SDLNet_TCP_DelSocket(socketSet, clientSockets[i]);
                SDLNet_TCP_Close(clientSockets[i]);
                // Shift remaining clients in the array
                for (int j = i; j < *numPlayers - 1; j++) {
                    clientSockets[j] = clientSockets[j + 1];
                }
                (*numPlayers)--;
                i--; // Adjust for shifted array
            } else {
                fprintf(stderr, "Error receiving data from client %d: %s\n", i, SDLNet_GetError());
            }
        }
    }
}


void sendDataToClients(TCPsocket* clientSockets, int numPlayers, Entity* players) {
    for (int i = 0; i < numPlayers; i++) {
        if (clientSockets[i] != NULL) {
            SDLNet_TCP_Send(clientSockets[i], players, sizeof(Entity) * numPlayers);
        }
    }
}

void sendDataToServer(TCPsocket clientSocket, Entity* players, int numPlayers) {
    SDLNet_TCP_Send(clientSocket, players, sizeof(Entity) * numPlayers);
}


void receiveDataFromServer(TCPsocket clientSocket, Entity* players, int numPlayers) {
    int len = SDLNet_TCP_Recv(clientSocket, players, sizeof(Entity) * numPlayers);
    if (len <= 0) {
        // Handle error or disconnection
        fprintf(stderr, "Error receiving data or server disconnected: %s\n", SDLNet_GetError());
        SDLNet_TCP_Close(clientSocket);
        SDLNet_Quit();
        exit(EXIT_FAILURE);
    }
}
