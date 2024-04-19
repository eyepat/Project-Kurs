#ifndef MODEL_H
#define MODEL_H

#include <stdbool.h>

// Define data structures for game entities and playing field

typedef struct {
    float x, y;
    int radius;
    float xSpeed,ySpeed;  
} Entity;

typedef struct {
    int width;
    int height;
} Field;


typedef struct {
    int startTime;
    int currentTime;
    int maxTime;
} Timer;


// Function declarations
void initializeGame(Entity *player, Entity *ball, Field *field);
void updatePlayerPosition(Entity *player, bool up, bool down, bool left, bool right, const Field *field, float deltaTime);
void updateBallPosition(Entity *ball, Entity *player, const Field *field, float deltaTime);
void initializeTimer(Timer* timer, int maxTime);
void updateTimer(Timer* timer);

#endif /* MODEL_H */
