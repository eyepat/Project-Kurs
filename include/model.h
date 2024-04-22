#ifndef MODEL_H
#define MODEL_H
#include <SDL2/SDL.h>
#include <stdbool.h>

// Define data structures for game entities and playing field

typedef struct {
    float x, y;
    int radius;
    float xSpeed,ySpeed; 
    int colorData[4]; // En array som innehåller all färginformation om objektet (RGB och opacity från 0-255)
} Entity;
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
void initializeGame(Entity *player, Entity *ball, Field *field, Entity *player2);
void updatePlayerPosition(Entity *player, bool up, bool down, bool left, bool right, const Field *field, float deltaTime);
void updateBallPosition(Entity *ball, Entity *player, Entity *player2, Field *field, Score *score, float deltaTime);
void resetGame(Entity *player, Entity *ball, Field *field, Entity *player2);
void initializeScore(Score *score);
void updateScore(Score *score, int teamNumber);
void initializeTimer(Timer *timer, int maxTime);
void updateTimer(Timer *timer);







#endif /* MODEL_H */
