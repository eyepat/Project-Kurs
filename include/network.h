#ifndef NETWORK_H
#define NETWORK_H

#include <SDL2/SDL_net.h>
#include "model.h" 
#include "view.h"


void initServer(IPaddress ip, GameState *gameState, Client clients[], SDLNet_SocketSet *socketSet, SDL_Renderer* renderer, TTF_Font* font, int windowWidth, MenuState* menuState, int windowHeight, int *portPointer, char hostIP[20],int* isServerPointer, bool* closeWindow);
void initClient(IPaddress ip, Client *clientInfo, GameState *gameState, SDL_Renderer* renderer, TTF_Font* font, int windowWidth, MenuState* menuState, int windowHeight, int *portPointer, char hostIP[20], int *isServer, bool *closeWindow);
void receiveDataFromClients(Client clients[], SDLNet_SocketSet socketSet, GameState *gameState);
void sendDataToClients(Client clients[], GameState *gameState);
void sendDataToServer(Client *clientInfo, GameState *gameState);
void receiveDataFromServer(Client *clientInfo, GameState *gameState);
// void acceptNewOrReconnectingClients(TCPsocket serverSocket, TCPsocket* clientSockets, SDLNet_SocketSet socketSet, GameState *gameState);
// void acceptNewOrReconnectingClients(TCPsocket serverSocket, TCPsocket* clientSockets, SDLNet_SocketSet socketSet, GameState *gameState);

#endif
