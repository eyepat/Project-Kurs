#ifndef MODEL_H
#define MODEL_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <stdbool.h>

#define PALYER_SPEED 500
#define MAX_PLAYERS 3
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800

// Entities Definitions 
typedef struct {
    float x, y;
    int radius;
    float xSpeed, ySpeed; 
    int colorData[4]; 
} Entity;

typedef struct {
    bool up;
    bool down;
    bool left;
    bool right;
} MovementFlags;

// Game Environment Definitions 
typedef struct {
    SDL_Rect box;
    int teamID;
} Goal;

typedef struct {
    int width;
    int height;
    Goal goals[2];
} Field;

//  Game State Definitions 
typedef struct {
    int startTime;
    int currentTime;
    int maxTime;
} Timer;

typedef struct {
    int team1Score;
    int team2Score;
} Score;

typedef struct {
    int numPlayers;
    Entity ball;
    Entity players[MAX_PLAYERS];
    Score scoreTracker;
    Timer gameTimer;
} GameState;

typedef struct {
    TCPsocket socket;
    int clientID;
    MovementFlags flags;
    int isActive;
} Client;

//  User Interface Definitions 
typedef struct {
    SDL_Rect bounds;
    SDL_Texture* texture;
} Button;

typedef struct {
    char username[40];
    char ip[16];
    int hostPort;
    char userInputIp[16];
    int userInputPort;
    int menuState; //menuIndex for buttons clicked 
    Button hostButton;
    Button joinButton;
    Button exitButton;
    Button startButton;
    Button joinHostButton;
    Button onlineButton;
    Button localButton;
    Button ipInputButton;
    Button backButton;
    SDL_Texture* menuBackground;
    SDL_Texture* gameBackground;
} MenuState;


//  Function Declarations 
void initializeGame(GameState *gameState, Field *field);
void updatePlayerPosition(GameState *gameState, Client clients[], const Field *field, float deltaTime);
int updateBallPosition(Entity *ball, GameState *gameState, Field *field, Score *score, float deltaTime, int *scoreFlag);
void initializeScore(Score* score);
void updateScore(Score *score, int teamNumber);
void initializeTimer(Timer* timer, int maxTime);
void updateTimer(Timer* timer);
void resetGame(GameState *gameState, Entity *ball, Field *field);
void initializePlayersPosition(GameState *gameState, Field *field);

#endif // MODEL_H
