#ifndef NETWORK_H
#define NETWORK_H

#include <SDL_net.h>
#include "model.h" 

void initServer(IPaddress ip, GameState *gameState, TCPsocket *serverSocket, TCPsocket clientSockets[], SDLNet_SocketSet *socketSet);
void initClient(IPaddress ip, GameState *gameState, TCPsocket *clientSocket);
void updateGameState(GameState *gameState, Entity *incomingPlayers);
void receiveDataFromClients(TCPsocket* clientSockets, SDLNet_SocketSet socketSet, GameState *gameState);
void sendDataToClients(TCPsocket* clientSockets, GameState *gameState);
void sendDataToServer(TCPsocket clientSocket, GameState *gameState);
void receiveDataFromServer(TCPsocket clientSocket, GameState *gameState);
void acceptNewOrReconnectingClients(TCPsocket serverSocket, TCPsocket* clientSockets, SDLNet_SocketSet socketSet, GameState *gameState);

#endif
