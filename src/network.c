#define MAX_PLAYERS 4
#include <stdio.h>      
#include <stdlib.h>    
#include <string.h>     
#include <SDL2/SDL_net.h> 
#include "network.h"
#include "model.h"

void updateGameState(Entity* players, int numPlayers, Entity* incomingPlayers) {
    for (int i = 0; i < numPlayers; i++) {
        players[i].x = incomingPlayers[i].x;
        players[i].y = incomingPlayers[i].y;
        players[i].xSpeed = incomingPlayers[i].xSpeed;
        players[i].ySpeed = incomingPlayers[i].ySpeed;
        memcpy(players[i].colorData, incomingPlayers[i].colorData, sizeof(players[i].colorData));
    }
}

void broadcastGameState(TCPsocket* clientSockets, int numClients, Entity* players, int numPlayers) {
    for (int i = 0; i < numClients; i++) {
        if (clientSockets[i] != NULL) {
            for (int j = 0; j < numPlayers; j++) {
                SDLNet_TCP_Send(clientSockets[i], &players[j], sizeof(Entity));
            }
        }
    }
}

void startServer(GameIPAddress ip, Entity *players, int *numPlayers) {
    TCPsocket serverSocket;
    SDLNet_SocketSet socketSet;
    TCPsocket clientSockets[MAX_PLAYERS]; 
    printf("Open the server socketbefore\n ");//troublehsoot

    // Open the server socket
    serverSocket = SDLNet_TCP_Open(&ip);// program bug it freezes here
    printf("Open the server socketafter\n ");

    if (!serverSocket) {
        fprintf(stderr, "Failed to open socket: %s\n", SDLNet_GetError());
        return;
    }

    printf(" Allocate and initialize the socket set\n ");
    // Allocate and initialize the socket set
    socketSet = SDLNet_AllocSocketSet(MAX_PLAYERS + 1); // +1 for the server socket
    if (!socketSet) {
        fprintf(stderr, "Failed to allocate socket set: %s\n", SDLNet_GetError());
        SDLNet_TCP_Close(serverSocket);
        return;
    }
    SDLNet_TCP_AddSocket(socketSet, serverSocket);
    printf(" Main server loop\n ");

    // Main server loop
    while (1) {
        // Check for activity on sockets
        if (SDLNet_CheckSockets(socketSet, 0) > 0) {
            // Check for incoming connections
            if (SDLNet_SocketReady(serverSocket)) {
                // Accept new connection
                TCPsocket newClient = SDLNet_TCP_Accept(serverSocket);
                if (newClient) {
                    // Check if max players limit reached
                    if (*numPlayers < MAX_PLAYERS) {
                        clientSockets[*numPlayers] = newClient;
                        SDLNet_TCP_AddSocket(socketSet, newClient);
                        (*numPlayers)++;
                        printf("Accepted new client connection. Total players: %d\n", *numPlayers);
                    } else {
                        printf("Max players limit reached. Rejecting new connection.\n");
                        SDLNet_TCP_Close(newClient);
                    }
                }
            }

            // Process each client socket
            for (int i = 0; i < *numPlayers; i++) {
                if (SDLNet_SocketReady(clientSockets[i])) {
                    Entity incomingPlayer;
                    int recvResult = SDLNet_TCP_Recv(clientSockets[i], &incomingPlayer, sizeof(Entity));
                    if (recvResult > 0) {
                        // Update received data and broadcast
                        updateGameState(players, *numPlayers, &incomingPlayer);
                        broadcastGameState(clientSockets, *numPlayers, players, *numPlayers);
                    } else if (recvResult == 0) {
                        // Client disconnected
                        printf("Client disconnected. Removing from game.\n");
                        SDLNet_TCP_DelSocket(socketSet, clientSockets[i]);
                        SDLNet_TCP_Close(clientSockets[i]);
                        // Shift remaining clients in array
                        for (int j = i; j < *numPlayers - 1; j++) {
                            clientSockets[j] = clientSockets[j + 1];
                        }
                        (*numPlayers)--;
                    } else {
                        fprintf(stderr, "Error receiving data from client %d: %s\n", i, SDLNet_GetError());
                    }
                }
            }
        }
    }

    // Clean up resources
    for (int i = 0; i < *numPlayers; i++) {
        if (clientSockets[i]) {
            SDLNet_TCP_DelSocket(socketSet, clientSockets[i]);
            SDLNet_TCP_Close(clientSockets[i]);
        }
    }
    SDLNet_FreeSocketSet(socketSet);
    SDLNet_TCP_Close(serverSocket);
}

void startClient(GameIPAddress ip, Entity *players, int numPlayers) {
    TCPsocket clientSocket;

    clientSocket = SDLNet_TCP_Open(&ip);
    if (!clientSocket) {
        fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        // exit(EXIT_FAILURE);
    }

    Entity incomingPlayers[numPlayers]; // Buffer to receive player updates
    while (1) {
        // Assume the server sends updates for all players in one go
        int len = SDLNet_TCP_Recv(clientSocket, incomingPlayers, sizeof(Entity) * numPlayers);
        if (len > 0) {
            // Process received data
            updateGameState(players, numPlayers, incomingPlayers);
        } else {
            // Handle error or disconnection
            break;
        }
    }

    SDLNet_TCP_Close(clientSocket);
    SDLNet_Quit();
}