#ifndef MODEL_H
#define MODEL_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

#define MAX_PLAYERS 4
#define PALYER_SPEED 350
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800
#define NUM_SOUNDS 5
#define FPS 60
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


typedef struct timer Timer;

typedef struct {
    int team1Score;
    int team2Score;
} Score;

typedef struct {
    int numPlayers;
    Entity ball;
    Entity players[MAX_PLAYERS];
    Score scoreTracker;
    Timer *gameTimer;
    bool isGameOver; 
    Field field;
} GameState;

typedef struct {
    TCPsocket socket;
    int clientID;
    MovementFlags flags;
    int isActive;
    int gameStart;
} Client;

//  User Interface Definitions 
typedef struct {
    SDL_Rect bounds;
    SDL_Texture* texture;
} Button;

typedef struct {
    int color[4];  // Lagets färg: RGB och opacity
    int score;     // Antal mål laget har gjort
    int goalIndex; // Index för det mål som tillhör detta lag
} Team;

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
void initializeSDL();
void initializeGame(GameState *gameState, Field *field);
void updatePlayerPosition(GameState *gameState, Client clients[], const Field *field, float deltaTime);
int updateBallPosition(Entity *ball, GameState *gameState, Field *field, Score *score, float deltaTime, int *scoreFlag);
void assignRandomColors(GameState *gameState);
void initializeScore(Score* score);
void updateScore(Score *score, int teamID);
Timer *createTimer(int startTime, int currentTime, int maxTime);
void destroyTimer(Timer *timer);
int getCurrentTime(const Timer *timer);
void updateTimer(Timer *timer, GameState *gameState);
void initializeTimer(Timer* timer, int maxTime);


void renderWinner(SDL_Renderer *renderer, TTF_Font *font, const Score *score);
void resetGameAfterGoal(GameState *gameState, Entity *ball, Field *field);
void resetGameState(GameState *gameState, Entity *ball, Field *field, Client clients[], int isServer, SDLNet_SocketSet socketSet);
void updatePlayerPositionLocal(GameState *gameState, const Field *field, float deltaTime, MovementFlags flags[2]);
void handleGameOver(bool *closeWindow, GameState *gameState, SDL_Renderer *renderer, TTF_Font *font, Field *field, int isServer, Client clients[], SDLNet_SocketSet socketSet);
#endif // MODEL_H
