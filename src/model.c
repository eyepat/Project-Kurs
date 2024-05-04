#include <math.h>
#include <SDL2/SDL.h>
#include "model.h"
#define SPEED 900

void initializeGame(GameState *gameState, Entity *ball, Field *field) {
    // Get the current display mode
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0) {
        SDL_Log("SDL_GetCurrentDisplayMode failed: %s", SDL_GetError());
        return; // Return or handle the error appropriately
    }

    // Set field dimensions based on the current display resolution
    field->width = displayMode.w;
    field->height = displayMode.h;

    const int GOAL_WIDTH = 80; // Replace with the actual value
    const int GOAL_HEIGHT = 250;

    // Initialize players' properties
   
    for (int i = 0; i < gameState->numPlayers; i++) {
        gameState->players[i].x = (i + 1) * (field->width / (gameState->numPlayers + 1));
        gameState->players[i].y = field->height / 2;
        gameState->players[i].radius = field->width / 128;
        if (i == 0) {
            modifyPlayerColors(255, 255, 0, 255, gameState->players[i].colorData);
        } else {
            modifyPlayerColors(0, 0, 255, 255, gameState->players[i].colorData);
        }
    }

    // Initialize ball properties
    ball->x = field->width / 2;
    ball->y = field->height / 2; 
    ball->radius = 20;
    ball->xSpeed = 0; // Initialize ball's speed in the x-direction to zero
    ball->ySpeed = 0; // Initialize ball's speed in the y-direction to zero

    // Initialize goal properties
    field->goals[0].box = (SDL_Rect){field->width * 0.063, (field->height * 1.03 - GOAL_HEIGHT) / 2, GOAL_WIDTH, GOAL_HEIGHT};
    field->goals[0].teamID = 1; // Team 1's goal
    field->goals[1].box = (SDL_Rect){field->width * 0.937 - GOAL_WIDTH, (field->height * 1.03 - GOAL_HEIGHT) / 2, GOAL_WIDTH, GOAL_HEIGHT};
    field->goals[1].teamID = 2; // Team 2's goal
    initializeScore(&gameState->score);
    initializeTimer(&gameState->timer, 120, SDL_GetTicks());   // Set game duration to 120 seconds
}

// Function to update the player's position based on user input
void updatePlayerPosition(GameState *gameState, MovementFlags flags[], const Field *field, float deltaTime) {
    for (int i = 0; i < gameState->numPlayers; i++) {
        float speed = SPEED * deltaTime;

        float verticalMargin = field->height * 0.12;
        float bottomMargin = field->height * 0.10;
        float horizontalMargin = field->width * 0.07;

        if (flags[i].up && gameState->players[i].y - gameState->players[i].radius > verticalMargin) {
            gameState->players[i].y -= speed;
        }
        if (flags[i].down && gameState->players[i].y + gameState->players[i].radius < field->height - bottomMargin) {
            gameState->players[i].y += speed;
        }
        if (flags[i].left && gameState->players[i].x - gameState->players[i].radius > horizontalMargin) {
            gameState->players[i].x -= speed;
        }
        if (flags[i].right && gameState->players[i].x + gameState->players[i].radius < field->width - horizontalMargin) {
            gameState->players[i].x += speed;
        }
    }
}


// Function to update ball position
int updateBallPosition(Entity *ball, GameState *gameState, Field *field, float deltaTime, int *scoreFlag) {    
    // Loop through all players to check for interactions with the ball
    for (int i = 0; i < gameState->numPlayers; i++) {
        Entity *player = &gameState->players[i];
        float distance = sqrt(pow(player->x - ball->x, 2) + pow(player->y - ball->y, 2));
        float kickThreshold = player->radius + ball->radius;

        if (distance < kickThreshold) {
            float dx = ball->x - player->x;
            float dy = ball->y - player->y;
            float magnitude = sqrt(dx * dx + dy * dy);
            float kickForce = 900.0f; // Kick force magnitude
            dx /= magnitude; // Normalize the direction vector
            dy /= magnitude;
           
            ball->xSpeed = dx * kickForce;
            ball->ySpeed = dy * kickForce;
        }
    }
    // Apply friction or deceleration to gradually stop the ball
    float deceleration = 400.0f; // Adjust this value as needed
    if (fabs(ball->xSpeed) > 0) {
        float direction;
        if (ball->xSpeed > 0) {
            direction = -1; // Ball is moving to the right
        } else {
            direction = 1; // Ball is moving to the left or stationary
        }
        float friction = deceleration * deltaTime;
        if (fabs(ball->xSpeed) < friction) {
            ball->xSpeed = 0; // If friction would stop the ball, set speed to 0
        } else {
            ball->xSpeed += friction * direction;
        }
    }
    if (fabs(ball->ySpeed) > 0) {
        float direction;
        if (ball->ySpeed > 0) {
            direction = -1; // Ball is moving downward
        } else {
            direction = 1; // Ball is moving upward or stationary
        }
        float friction = deceleration * deltaTime;
        if (fabs(ball->ySpeed) < friction) {
            ball->ySpeed = 0; // If friction would stop the ball, set speed to 0
        } else {
            ball->ySpeed += friction * direction;
        }
    }

    // Update the ball's position based on its speed
    ball->x += ball->xSpeed * deltaTime;
    ball->y += ball->ySpeed * deltaTime;
  // Kontrollera om bollen har nått målområdet
    for (int i = 0; i < 2; i++) {
        if (ball->x - ball->radius > field->goals[i].box.x &&
            ball->x + ball->radius < field->goals[i].box.x + field->goals[i].box.w &&
            ball->y - ball->radius > field->goals[i].box.y &&
            ball->y + ball->radius < field->goals[i].box.y + field->goals[i].box.h && *scoreFlag !=1 ) {
            // Bollen är i målet
            ball->xSpeed = 0;
            ball->ySpeed = 0;
            int scoringTeam = (field->goals[i].teamID == 1) ? 2 : 1;
            updateScore(&gameState->score, scoringTeam); //score within gameState
            *scoreFlag = 1;
           
            break;
        }
  
    }
      // Check boundaries and apply the margins set for the player
    float verticalMargin = field->height * 0.12; // Top margin
    float bottomMargin = field->height * 0.10; // Bottom margin
    float horizontalMargin = field->width * 0.07; // Side margins dvs left right

    // Check if the ball hits the left or right wall
    if (ball->x - ball->radius <= horizontalMargin || ball->x + ball->radius >= field->width - horizontalMargin) {
        ball->xSpeed *= -1;
        ball->x = fmax(ball->radius + horizontalMargin, fmin(ball->x, field->width - ball->radius - horizontalMargin));
    }

    // Check if the ball hits the top or bottom wall
    if (ball->y - ball->radius <= verticalMargin || ball->y + ball->radius >= field->height - bottomMargin) {
        ball->ySpeed *= -1;
        ball->y = fmax(ball->radius + verticalMargin, fmin(ball->y, field->height - ball->radius - bottomMargin));
    }

    return 1;
}

void initializeScore(Score* score) {
    score->team1Score = 0;
    score->team2Score = 0;
}

void updateScore(Score *score, int teamNumber) {
    if (teamNumber == 1) {
        score->team1Score++;
    } else if (teamNumber == 2) {
        score->team2Score++;
    }
}

void initializeTimer(Timer* timer, int maxTime, Uint32 startTime) {
    if (startTime == 0) {
        timer->startTime = SDL_GetTicks();  // Default to current tick if not provided
    } else {
        timer->startTime = startTime;      // Use provided startTime for synchronization
    }
    timer->currentTime = SDL_GetTicks() - timer->startTime;
    timer->maxTime = maxTime;
}


void updateTimer(GameState *gameState) {
    unsigned int currentTicks = SDL_GetTicks();
    gameState->timer.currentTime = (currentTicks - gameState->timer.startTime) / 1000;  // Convert milliseconds to seconds
    if (gameState->timer.currentTime >= gameState->timer.maxTime) {
       initializeTimer(&gameState->timer, gameState->timer.maxTime, SDL_GetTicks()); // Reset with current time

    }
}

void resetGame(GameState *gameState, Entity *ball, Field *field) {
    for (int i = 0; i < gameState->numPlayers; i++) {
        gameState->players[i].x = (i % 2 == 0) ? field->width / 4 : field->width * 3 / 4;
        gameState->players[i].y = field->height / 2;
    }

    // Reset ball position
    ball->x = field->width / 2;
    ball->y = field->height / 2;
    ball->xSpeed = 0;
    ball->ySpeed = 0;

    // Goals remain unchanged 
}






