#ifndef NETWORK_H
#define NETWORK_H
#include <SDL2/SDL_net.h>
#include "model.h" 

void initServer(IPaddress ip, Entity *players, int *numPlayers, TCPsocket *serverSocket, TCPsocket clientSockets[], SDLNet_SocketSet *socketSet);
void initClient(IPaddress ip, Entity *players, int numPlayers, TCPsocket *clientSocket);
void updateGameState(Entity* players, int numPlayers, Entity* incomingPlayers);
void receiveDataFromClients(TCPsocket* clientSockets, SDLNet_SocketSet socketSet, Entity* players, int* numPlayers);
void sendDataToClients(TCPsocket* clientSockets, int numPlayers, Entity* players);
void sendDataToServer(TCPsocket clientSocket, Entity* players, int numPlayers);
void receiveDataFromServer(TCPsocket clientSocket, Entity* players, int numPlayers);
 


#endif