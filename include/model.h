#ifndef MODEL_H
#define MODEL_H
#include <SDL2/SDL.h>
#include <stdbool.h>
#define MAX_PLAYERS 2

// Define data structures for game entities and playing field

typedef struct {
    float x, y;
    int radius;
    float xSpeed,ySpeed; 
    int colorData[4]; // En array som innehåller all färginformation om objektet (RGB och opacity från 0-255)
} Entity;

typedef struct {
    int numPlayers;
    Entity players[MAX_PLAYERS];
} GameState;


typedef struct {
    bool up;
    bool down;
    bool left;
    bool right;
} MovementFlags;



typedef struct {
    SDL_Rect box;
    int teamID; // 1 eller 2 för att identifiera vilket lag som ska få poäng vid träff
} Goal;

typedef struct {
    int width;
    int height;
    Goal goals[2]; // Antag två mål, ett för varje lag
} Field;

typedef struct {
    int startTime;
    int currentTime;
    int maxTime;
} Timer;

typedef struct {
    int team1Score;
    int team2Score;
} Score;


// Function declarations
// void initializeGame(Entity players[], int numPlayers, Entity *ball, Field *field);
// void updatePlayerPosition(Entity players[], MovementFlags flags[], int numPlayers, const Field *field, float deltaTime);
// int updateBallPosition(Entity *ball, Entity players[], int numPlayers, Field *field, Score *score, float deltaTime, int *scoreFlag);
// void resetGame(Entity players[], Entity *ball, Field *field, int numPlayers);
// void initializeScore(Score *score);
// void updateScore(Score *score, int teamNumber);
// void initializeTimer(Timer *timer, int maxTime);
// void updateTimer(Timer *timer);


void initializeGame(GameState *gameState, Entity *ball, Field *field);
void updatePlayerPosition(GameState *gameState, MovementFlags flags[], const Field *field, float deltaTime);
int updateBallPosition(Entity *ball, GameState *gameState, Field *field, Score *score, float deltaTime, int *scoreFlag);
void initializeScore(Score* score);
void updateScore(Score *score, int teamNumber);
void initializeTimer(Timer* timer, int maxTime);
void updateTimer(Timer* timer);
void resetGame(GameState *gameState, Entity *ball, Field *field);


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
    SDL_Texture* menuBackground;
    SDL_Texture* gameBackground;
} MenuState;



#endif /* MODEL_H */
