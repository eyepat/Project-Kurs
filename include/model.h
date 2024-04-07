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

// Function declarations
void initializeGame(Entity *player, Entity *ball, Field *field);
void updatePlayerPosition(Entity *player, bool up, bool down, bool left, bool right, const Field *field, float deltaTime);
void updateBallPosition(Entity *ball, Entity *player, const Field *field, float deltaTime);

#endif /* MODEL_H */
