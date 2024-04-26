#ifndef NETWORK_H
#define NETWORK_H
#include <SDL2/SDL_net.h>
#include "model.h" 

typedef struct {
    Uint32 host;    // IP address
    Uint16 port;    // Port number
} GameIPAddress;

void updateGameState(Entity* players, int numPlayers, Entity* incomingPlayers);
void broadcastGameState(TCPsocket* clientSockets, int numClients, Entity* players, int numPlayers);
void startServer(GameIPAddress ip, Entity *players, int *numPlayers);
void startClient(GameIPAddress ip, Entity *players, int numPlayers);

#endif