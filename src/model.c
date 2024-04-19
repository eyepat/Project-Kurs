#include "model.h"
#include <math.h>
#include <SDL2/SDL.h>
#define SPEED 900

// Function to initialize the game
void initializeGame(Entity *player, Entity *ball, Field *field) {
    // Get the current display mode
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0) {
        SDL_Log("SDL_GetCurrentDisplayMode failed: %s", SDL_GetError());
        return; // Return or handle the error appropriately
    }

    // Set field dimensions based on the current display resolution
    field->width = displayMode.w;
    field->height = displayMode.h;

    // Initialize player properties
    player->x = field->width / 2;
    player->y = field->height / 1.95;
    player->radius = field->width / 128;

    // Initialize ball properties
    ball->x = field->width / 4;
    ball->y = field->height / 1.95;
    ball->radius = 20;
    ball->xSpeed = 0; // Initialize ball's speed in the x-direction to zero
    ball->ySpeed = 0; // Initialize ball's speed in the y-direction to zero
}


// Function to update the player's position based on user input
void updatePlayerPosition(Entity *player, bool up, bool down, bool left, bool right, const Field *field, float deltaTime) {
    float speed = SPEED * deltaTime;
    if (up) {
        player->y -= speed;
        if (player->y < player->radius + field->height * 0.11) {
            player->y = player->radius + field->height * 0.11;
        }
    }

    if (down) {
        player->y += speed;
        if (player->y > field->height - player->radius - field->height * 0.09) {
            player->y = field->height - player->radius - field->height * 0.09;
        }
    }

    if (left) {
        player->x -= speed;
        if (player->x < player->radius + field->height * 0.11) {
            player->x = player->radius + field->height * 0.11;
        }
    }

    if (right) {
        player->x += speed;
        if (player->x > field->width - player->radius - field->height * 0.11) {
            player->x = field->width - player->radius - field->height * 0.11;
        }
    }
}



void updateBallPosition(Entity *ball, Entity *player, const Field *field, float deltaTime) {
    // Calculate the distance between the ball and the player
    float distance = sqrt(pow(player->x - ball->x, 2) + pow(player->y - ball->y, 2));

    // Define a threshold for player-ball collision
    float kickThreshold = player->radius + ball->radius*2;

    // If the distance is less than the kick threshold, the player has kicked the ball
    if (distance < kickThreshold) {
        // Calculate the direction from player to ball
        float dx = ball->x - player->x;
        float dy = ball->y - player->y;
        float magnitude = sqrt(dx * dx + dy * dy);

        // Normalize the direction
        dx /= magnitude;
        dy /= magnitude;

        // Apply kicking force with a higher magnitude for faster kick
        float kickMagnitude = 1300.0f; // Adjust the kick magnitude for faster kick
        ball->xSpeed = dx * kickMagnitude;
        ball->ySpeed = dy * kickMagnitude;
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

      // Check boundaries and apply the margins set for the player
    float verticalMargin = field->height * 0.12; // Top margin
    float bottomMargin = field->height * 0.09; // Bottom margin
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
}

void initializeTimer(Timer* timer, int maxTime) {
    timer->startTime = SDL_GetTicks();
    timer->currentTime = 0;
    timer->maxTime = maxTime;
}

void updateTimer(Timer* timer) {
    unsigned int currentTicks = SDL_GetTicks();
    timer->currentTime = (currentTicks - timer->startTime) / 1000; //change to seconds
    if (timer->currentTime >= timer->maxTime) {
        initializeTimer(timer, timer->maxTime);
    }
}








