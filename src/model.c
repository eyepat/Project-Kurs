

#include "model.h"
#include "view.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <SDL2/SDL.h>


void initializeGame(GameState *gameState, Field *field) {
    // Set field dimensions based on the current display resolution
    field->width = SCREEN_WIDTH;
    field->height = SCREEN_HEIGHT;

    const int GOAL_WIDTH = 80; // Replace with the actual value
    const int GOAL_HEIGHT = 250;

    // Initialize players' properties
    for (int i = 0; i < gameState->numPlayers; i++) {
        gameState->players[i].x = (i + 1) * (field->width / (gameState->numPlayers + 1));
        gameState->players[i].y = field->height / 4;
        gameState->players[i].radius = field->width / 128;
    }

    // Initialize ball properties
    gameState->ball.x = field->width / 2;
    gameState->ball.y = field->height / 2;
    gameState->ball.radius = 20;
    gameState->ball.xSpeed = 0; // Initialize ball's speed in the x-direction to zero
    gameState->ball.ySpeed = 0; // Initialize ball's speed in the y-direction to zero

    assignRandomColors(gameState); 

    // Initialize goal properties
    field->goals[0].box = (SDL_Rect){field->width * 0.063, (field->height * 1.03 - GOAL_HEIGHT) / 2, GOAL_WIDTH, GOAL_HEIGHT};
    field->goals[0].teamID = 1; // Team 1's goal
    field->goals[1].box = (SDL_Rect){field->width * 0.937 - GOAL_WIDTH, (field->height * 1.03 - GOAL_HEIGHT) / 2, GOAL_WIDTH, GOAL_HEIGHT};
    field->goals[1].teamID = 2; // Team 2's goal
}







void updatePlayerPosition(GameState *gameState, Client clients[], const Field *field, float deltaTime) {
    for (int i = 0; i < gameState->numPlayers; i++) {
        // Ensure that the client index used is within the bounds of the gameState players array
        if (clients[i].clientID < 0 || clients[i].clientID >= gameState->numPlayers) {
            continue;  // Skip this client if their clientID is out of range
        }

        // if (!clients[i].isActive) {
        //     continue; // Skip inactive clients
        // }

        // Use clientID to refer to the specific player in gameState
        int clientID = clients[i].clientID;
        MovementFlags clientFlags = clients[i].flags;
        float speed = PALYER_SPEED * deltaTime;

        float verticalMargin = field->height * 0.12;
        float bottomMargin = field->height * 0.10;
        float horizontalMargin = field->width * 0.07;

        if (clientFlags.up && gameState->players[clientID].y - gameState->players[clientID].radius > verticalMargin) {
            gameState->players[clientID].y -= speed;
        }
        if (clientFlags.down && gameState->players[clientID].y + gameState->players[clientID].radius < field->height - bottomMargin) {
            gameState->players[clientID].y += speed;
        }
        if (clientFlags.left && gameState->players[clientID].x - gameState->players[clientID].radius > horizontalMargin) {
            gameState->players[clientID].x -= speed;
        }
        if (clientFlags.right && gameState->players[clientID].x + gameState->players[clientID].radius < field->width - horizontalMargin) {
            gameState->players[clientID].x += speed;
        }
    }
}



// Function to update ball position
int updateBallPosition(Entity *ball, GameState *gameState, Field *field, Score *score, float deltaTime, int *scoreFlag) {
    // Loop through all players to check for interactions with the ball
    for (int i = 0; i < gameState->numPlayers; i++) {
        Entity *player = &gameState->players[i];
        float distance = sqrt(pow(player->x - ball->x, 2) + pow(player->y - ball->y, 2));
        float kickThreshold = player->radius + ball->radius;

        if (distance < kickThreshold) {
            float dx = ball->x - player->x;
            float dy = ball->y - player->y;
            float magnitude = sqrt(dx * dx + dy * dy);

            // Prevent division by zero
            if (magnitude != 0) {
                dx /= magnitude; // Normalize the direction vector
                dy /= magnitude;
                float kickForce = 900.0f; // Kick force magnitude
                ball->xSpeed = dx * kickForce;
                ball->ySpeed = dy * kickForce;
            }
        }
    }

    // Apply friction or deceleration to gradually stop the ball
    float deceleration = 400.0f; // Adjust this value as needed
    float friction = deceleration * deltaTime;

    // Apply friction to the ball's xSpeed
    if (fabs(ball->xSpeed) > 0) {
        float direction = (ball->xSpeed > 0) ? -1 : 1;
        if (fabs(ball->xSpeed) < friction) {
            ball->xSpeed = 0; // If friction would stop the ball, set speed to 0
        } else {
            ball->xSpeed += friction * direction;
        }
    }

    // Apply friction to the ball's ySpeed
    if (fabs(ball->ySpeed) > 0) {
        float direction = (ball->ySpeed > 0) ? -1 : 1;
        if (fabs(ball->ySpeed) < friction) {
            ball->ySpeed = 0; // If friction would stop the ball, set speed to 0
        } else {
            ball->ySpeed += friction * direction;
        }
    }

    // Update the ball's position based on its speed
    ball->x += ball->xSpeed * deltaTime;
    ball->y += ball->ySpeed * deltaTime;

    // Check if the ball has reached the goal area
    for (int i = 0; i < 2; i++) {
        if (ball->x - ball->radius > field->goals[i].box.x &&
            ball->x + ball->radius < field->goals[i].box.x + field->goals[i].box.w &&
            ball->y - ball->radius > field->goals[i].box.y &&
            ball->y + ball->radius < field->goals[i].box.y + field->goals[i].box.h && *scoreFlag != 1) {
            // Ball is in the goal
            ball->xSpeed = 0;
            ball->ySpeed = 0;
            int scoringTeam = (field->goals[i].teamID == 1) ? 2 : 1;
            updateScore(score, scoringTeam);
            *scoreFlag = 1;
            break;
        }
    }

    // Check boundaries and apply the margins set for the player
    float verticalMargin = field->height * 0.12; // Top margin
    float bottomMargin = field->height * 0.10; // Bottom margin
    float horizontalMargin = field->width * 0.07; // Side margins

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

void assignRandomColors(GameState *gameState) {
    // Define colors for the teams
    int red[4] = {255, 120, 77, 200};  // RGBA for red with 200 opacity
    int blue[4] = {102, 102, 255, 200}; // RGBA for blue with 200 opacity
    int wineRed[4] = {255, 0, 0, 200};
    int navyBlue[4] ={100, 0, 180, 200};

    for (int i = 0; i < gameState->numPlayers; i++) {
        if (gameState->numPlayers == 2) {
            if (i == 0) {  // First player gets red
                gameState->players[i].colorData[0] = red[0];
                gameState->players[i].colorData[1] = red[1];
                gameState->players[i].colorData[2] = red[2];
                gameState->players[i].colorData[3] = red[3];
            } else {  // Second player gets blue
                gameState->players[i].colorData[0] = blue[0];
                gameState->players[i].colorData[1] = blue[1];
                gameState->players[i].colorData[2] = blue[2];
                gameState->players[i].colorData[3] = blue[3];
            }
        } else if (gameState->numPlayers == 4) {
            if (i == 0) {  // First two players get red
                gameState->players[i].colorData[0] = red[0];
                gameState->players[i].colorData[1] = red[1];
                gameState->players[i].colorData[2] = red[2];
                gameState->players[i].colorData[3] = red[3];
            }
            else if (i == 1)
            {
                gameState->players[i].colorData[0] = wineRed[0];
                gameState->players[i].colorData[1] = wineRed[1];
                gameState->players[i].colorData[2] = wineRed[2];
                gameState->players[i].colorData[3] = wineRed[3];
            }
            
            else if (i == 2)
            {  // Next two players get blue
                gameState->players[i].colorData[0] = blue[0];
                gameState->players[i].colorData[1] = blue[1];
                gameState->players[i].colorData[2] = blue[2];
                gameState->players[i].colorData[3] = blue[3];
            }
            else if (i == 3)
            {
                gameState->players[i].colorData[0] = navyBlue[0];
                gameState->players[i].colorData[1] = navyBlue[1];
                gameState->players[i].colorData[2] = navyBlue[2];
                gameState->players[i].colorData[3] = navyBlue[3];         
            }
            
        }
    }
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

void initializeTimer(Timer* timer, int maxTime) {
    timer->currentTime = 0;
    timer->startTime = SDL_GetTicks64();
    timer->maxTime = maxTime;
}

void updateTimer(Timer* timer, GameState *gameState) {
    unsigned int currentTicks = SDL_GetTicks();
    timer->currentTime = (currentTicks - timer->startTime) / 1000;  // Convert milliseconds to seconds

    if (timer->currentTime >= timer->maxTime) {
        timer->currentTime = timer->maxTime;  // Clamp the currentTime to maxTime to avoid overflow
        gameState->isGameOver = true;  // Set the game over flag when time is up
        printf("Time's up! Game over.\n");
    }
}

void renderWinner(SDL_Renderer *renderer, TTF_Font *font, const Score *score) {
    char message[100];
    SDL_Color color = {255, 255, 255};  // White color
    if (score->team1Score > score->team2Score) {
        sprintf(message, "Team 1 wins with a score of %d to %d!", score->team1Score, score->team2Score);
    } else if (score->team2Score > score->team1Score) {
        sprintf(message, "Team 2 wins with a score of %d to %d!", score->team2Score, score->team1Score);
    } else {
        sprintf(message, "The game is a draw, each team scoring %d.", score->team1Score);
    }

    SDL_Surface *surface = TTF_RenderText_Solid(font, message, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect textRect = {100, SCREEN_HEIGHT / 2 - surface->h / 2, surface->w, surface->h};  // Centered
    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}


void resetGame(GameState *gameState, Entity *ball, Field *field) {
    // Reset players' positions
    for (int i = 0; i < gameState->numPlayers; i++) {
        gameState->players[i].x = (i + 1) * (field->width / (gameState->numPlayers + 1));
        gameState->players[i].y = field->height / 4;
        gameState->players[i].radius = field->width / 128;
    }

    // Reset the ball position to the center of the field
    ball->x = field->width / 2;
    ball->y = field->height / 2;
    ball->xSpeed = 0; // Reset ball speed to zero
    ball->ySpeed = 0;

    // Optionally reset the radius of each player if the field dimensions might change
    int playerRadius = field->width / 128; // Set the player radius relative to the field width
    for (int i = 0; i < gameState->numPlayers; i++) {
        gameState->players[i].radius = playerRadius;
    }
}


void updatePlayerPositionLocal(GameState *gameState, const Field *field, float deltaTime, MovementFlags flags[2]) {
    for (int i = 0; i < gameState->numPlayers; i++) {
    

        MovementFlags clientFlags = flags[i];
        float speed = PALYER_SPEED * deltaTime;

        float verticalMargin = field->height * 0.12;
        float bottomMargin = field->height * 0.10;
        float horizontalMargin = field->width * 0.07;

        if (clientFlags.up && gameState->players[i].y - gameState->players[i].radius > verticalMargin) {
            gameState->players[i].y -= speed;
        }
        if (clientFlags.down && gameState->players[i].y + gameState->players[i].radius < field->height - bottomMargin) {
            gameState->players[i].y += speed;
        }
        if (clientFlags.left && gameState->players[i].x - gameState->players[i].radius > horizontalMargin) {
            gameState->players[i].x -= speed;
        }
        if (clientFlags.right && gameState->players[i].x + gameState->players[i].radius < field->width - horizontalMargin) {
            gameState->players[i].x += speed;
        }
        
        
    }
}



