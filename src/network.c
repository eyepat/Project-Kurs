#include <stdio.h>      
#include <stdlib.h>    
#include <string.h>     
#include <SDL2/SDL_net.h> 
#include "model.h"
#include "view.h"
#include "controller.h"
#include "network.h"


void initServer(IPaddress ip, GameState *gameState, Client clients[], SDLNet_SocketSet *socketSet, SDL_Renderer* renderer, TTF_Font* font, int windowWidth, MenuState* menuState, int windowHeight, int *portPointer, char hostIP[20],int* isServerPointer, bool* closeWindow) {
    printf("Initializing server\n");
    menuState->menuState = 11;

    // Open the server socket
    TCPsocket serverSocket = SDLNet_TCP_Open(&ip);
    if (!serverSocket) {
        fprintf(stderr, "Failed to open server socket: %s\n", SDLNet_GetError());
        SDLNet_TCP_Close(serverSocket);
        exit(EXIT_FAILURE);
    }

    // Allocate and initialize the socket set with size MAX_PLAYERS + 1 for the server socket
    *socketSet = SDLNet_AllocSocketSet(MAX_PLAYERS + 1);
    if (!*socketSet) {
        fprintf(stderr, "Failed to allocate socket set: %s\n", SDLNet_GetError());
        SDLNet_TCP_Close(serverSocket);
        exit(EXIT_FAILURE);
    }

    SDLNet_TCP_AddSocket(*socketSet, serverSocket);
    printf("Server initialized and listening\n");

    // Initialize client array
    for (int i = 0; i < MAX_PLAYERS; i++) {
        clients[i].socket = NULL;
        clients[i].clientID = 0;  
        clients[i].isActive = 0;
    }

    // Main server loop, accepting clients
    gameState->numPlayers = 1;
    while(gameState->numPlayers < MAX_PLAYERS) {

        handleMenuEvent(closeWindow, menuState);
        SDL_RenderClear(renderer);
        drawMenu(renderer, font, menuState, windowWidth, windowHeight, portPointer, hostIP, isServerPointer);
        SDL_RenderPresent(renderer);
        SDL_Delay(11);

        TCPsocket newClientSocket = SDLNet_TCP_Accept(serverSocket);
        if (newClientSocket) {
            menuState->menuState = (11 + gameState->numPlayers);
            clients[gameState->numPlayers].socket = newClientSocket;
            clients[gameState->numPlayers].clientID = gameState->numPlayers;  // Assign client ID
            clients[gameState->numPlayers].isActive = 1;  // Mark as active

            SDLNet_TCP_AddSocket(*socketSet, newClientSocket);

            // Send the client ID to the new client
            SDLNet_TCP_Send(newClientSocket, &clients[gameState->numPlayers].clientID, sizeof(clients[gameState->numPlayers].clientID));

            printf("New client connected with ID %d. Total Players: %d\n", clients[gameState->numPlayers].clientID, gameState->numPlayers + 1);
            gameState->numPlayers++;
        }
    }

    // Ensure we send the server full signal and the initial game state
    int serverFullSignal = 1;
    printf("Server is full, sending initial game state to all clients.\n");
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (clients[i].isActive && clients[i].socket != NULL) {
            SDLNet_TCP_Send(clients[i].socket, &serverFullSignal, sizeof(serverFullSignal));
            SDLNet_TCP_Send(clients[i].socket, gameState, sizeof(GameState));  // Send the initial game state
        }
    }
    printf("All players connected. Game starting.\n");
}


void initClient(IPaddress ip, Client *clientInfo, GameState *gameState, SDL_Renderer* renderer, TTF_Font* font, int windowWidth, MenuState* menuState, int windowHeight, int *portPointer, char hostIP[20], int *isServer, bool *closeWindow) {
    printf("Initializing client...\n");
    int showWaitScreen = 1;

    // Open the client socket
    clientInfo->socket = SDLNet_TCP_Open(&ip);
    if (!clientInfo->socket) {
        fprintf(stderr, "Failed to open client socket: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        exit(EXIT_FAILURE);
    }
    // Receive the client ID assigned by the server
    int result = SDLNet_TCP_Recv(clientInfo->socket, &clientInfo->clientID, sizeof(clientInfo->clientID));
    if (result > 0) {
        printf("Received client ID: %d\n", clientInfo->clientID);
        clientInfo->isActive = 1; 
        showWaitScreen = 1; 
    } else {
        fprintf(stderr, "Error receiving client ID. Result: %d, Error: %s\n", result, SDLNet_GetError());
        SDLNet_TCP_Close(clientInfo->socket);
        SDLNet_Quit();
        exit(EXIT_FAILURE);
    }

    SDLNet_SocketSet set;
    set = SDLNet_AllocSocketSet(1);
    SDLNet_TCP_AddSocket(set, clientInfo->socket);

    printf("Client connected to server.\n");

    while (showWaitScreen == 1)
    {
        menuState->menuState = 16;
        handleMenuEvent(closeWindow, menuState);
        SDL_RenderClear(renderer);
        drawMenu(renderer, font, menuState, windowWidth, windowHeight, portPointer, hostIP, isServer);
        SDL_RenderPresent(renderer);
        SDL_Delay(11);
    
        int serverFullSignal;
        if (SDLNet_CheckSockets(set, 1) > 0)
        {
            result = SDLNet_TCP_Recv(clientInfo->socket, &serverFullSignal, sizeof(serverFullSignal));
            if (result > 0) {
                if (serverFullSignal == 1) {
                    printf("Server is full. Waiting for initial game state...\n");
                    showWaitScreen = 0;
                    result = SDLNet_TCP_Recv(clientInfo->socket, gameState, sizeof(GameState));  // Receive the initial game state
                    if (result > 0) {
                        printf("Initial game state received. Ready to start the game.\n");
                    } else {
                        fprintf(stderr, "Error receiving initial game state. Result: %d, Error: %s\n", result, SDLNet_GetError());
                        SDLNet_TCP_Close(clientInfo->socket);
                        SDLNet_Quit();
                        exit(EXIT_FAILURE);
                    }
                }
            } 
            else {
                fprintf(stderr, "Error receiving server full signal. Result: %d, Error: %s\n", result, SDLNet_GetError());
                SDLNet_TCP_Close(clientInfo->socket);
                SDLNet_Quit();
                exit(EXIT_FAILURE);
            }
        }
    }
    printf("Client setup complete, ready for next steps.\n");
}


void sendDataToServer(Client *clientInfo, GameState *gameState) {
    int retries = 3; // Number of retry attempts
    int len;
    // Retry sending the clientID
    while (retries > 0) {
        // Send the clientID first
        len = SDLNet_TCP_Send(clientInfo->socket, &clientInfo->clientID, sizeof(int));
        if (len == sizeof(int)) {
            break; // Sent successfully, exit the loop
        } else {
            fprintf(stderr, "Failed to send complete clientID: %s\n", SDLNet_GetError());
            retries--; // Decrement the retry counter
        }
    }
    // Check if all retries failed
    if (retries == 0) {
        fprintf(stderr, "Failed to send clientID after multiple retries.\n");
        return; // Return early if failed to send clientID
    }

    // Now send the movement flags
    len = SDLNet_TCP_Send(clientInfo->socket, &clientInfo->flags, sizeof(MovementFlags));
    if (len < sizeof(MovementFlags)) {
        fprintf(stderr, "Failed to send complete MovementFlags: %s\n", SDLNet_GetError());
    }
}

void receiveDataFromClients(Client clients[], SDLNet_SocketSet socketSet, GameState *gameState) {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (clients[i].isActive) {
            // Receive the clientID first
            int clientID = 0;

            int recvResult = SDLNet_TCP_Recv(clients[i].socket, &clientID, sizeof(int));
         
            if (recvResult <= 0) {
                // Handle disconnection or error
                if (recvResult == 0) {
                    printf("Client %d disconnected. Slot kept open for reconnection.\n", clients[i].clientID);
                } else {
                    fprintf(stderr, "Error receiving clientID from client %d: %s\n", clients[i].clientID, SDLNet_GetError());
                }
                SDLNet_TCP_DelSocket(socketSet, clients[i].socket); // Remove the socket from the set
                SDLNet_TCP_Close(clients[i].socket);
                clients[i].isActive = 0; // Mark the client slot as inactive for reconnection
                continue; // Move to next client
            }
            
            // Validate the received clientID
            if (clientID >= 0 && clientID < MAX_PLAYERS) {
                // Proceed only if the clientID is within the valid range
                MovementFlags incomingFlags;
                incomingFlags.up = false;
                incomingFlags.down = false;
                incomingFlags.left = false;
                incomingFlags.right = false;
                    
                recvResult = SDLNet_TCP_Recv(clients[i].socket, &incomingFlags, sizeof(MovementFlags));
                if (recvResult > 0) {
                    // Update the movement flags for the corresponding client
                    clients[clientID].clientID = clientID; // Store clientID in the correct array slot
                    clients[clientID].flags = incomingFlags;
    

                } else if (recvResult == 0) {
                    printf("Client %d disconnected. Slot kept open for reconnection.\n", clientID);
                    SDLNet_TCP_DelSocket(socketSet, clients[i].socket); // Remove the socket from the set
                    SDLNet_TCP_Close(clients[i].socket);
                    clients[i].isActive = 0; // Mark the client slot as inactive for reconnection
                } else {
                    fprintf(stderr, "Error receiving data from client %d: %s\n", clientID, SDLNet_GetError());
                }
            } 
        }
    }
}

void sendDataToClients(Client clients[], GameState *gameState) {
    for (int i = 0; i < gameState->numPlayers; i++) {
        if (clients[i].isActive) {
            SDLNet_TCP_Send(clients[i].socket, gameState, sizeof(GameState));
        }
    }
}

void receiveDataFromServer(Client *clientInfo, GameState *gameState) {
    int expectedLen = sizeof(GameState);
    int receivedLen = SDLNet_TCP_Recv(clientInfo->socket, gameState, expectedLen);
    if (receivedLen < expectedLen) {
        fprintf(stderr, "Error receiving data or server disconnected: %s\n", SDLNet_GetError());
        SDLNet_TCP_Close(clientInfo->socket);
        SDLNet_Quit();
        exit(EXIT_FAILURE);
    }
}

