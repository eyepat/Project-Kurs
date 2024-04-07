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
    player->y = field->height / 2;
    player->radius = field->width / 128;

    // Initialize ball properties
    ball->x = field->width / 4;
    ball->y = field->height / 2;
    ball->radius = 20;
    ball->xSpeed = 0; // Initialize ball's speed in the x-direction to zero
    ball->ySpeed = 0; // Initialize ball's speed in the y-direction to zero
}


// Function to update the player's position based on user input
void updatePlayerPosition(Entity *player, bool up, bool down, bool left, bool right, const Field *field, float deltaTime) {
    float speed = SPEED * deltaTime;

    if (up) player->y -= speed;
    if (down) player->y += speed;
    if (left) player->x -= speed;
    if (right) player->x += speed;

    // Keep the player within the field boundaries
    player->x = fmax(player->radius, fmin(player->x, field->width - player->radius));
    player->y = fmax(player->radius, fmin(player->y, field->height - player->radius));
}








void updateBallPosition(Entity *ball, Entity *player, const Field *field, float deltaTime) {
    // Calculate the distance between the ball and the player
    float distance = sqrt(pow(player->x - ball->x, 2) + pow(player->y - ball->y, 2));

    // Define a threshold for player-ball collision
    float kickThreshold = player->radius + ball->radius;

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
        float direction = ball->xSpeed > 0 ? -1 : 1;
        float friction = deceleration * deltaTime;
        if (fabs(ball->xSpeed) < friction) {
            ball->xSpeed = 0; // If friction would stop the ball, set speed to 0
        } else {
            ball->xSpeed += friction * direction;
        }
    }
    if (fabs(ball->ySpeed) > 0) {
        float direction = ball->ySpeed > 0 ? -1 : 1;
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

    // Check if the ball hits the left or right wall
    if (ball->x - ball->radius <= 0 || ball->x + ball->radius >= field->width) {
        // Reverse the horizontal direction of the ball and adjust position to stay within bounds
        ball->xSpeed *= -1;
        ball->x = fmax(ball->radius, fmin(ball->x, field->width - ball->radius));
    }

    // Check if the ball hits the top or bottom wall
    if (ball->y - ball->radius <= 0 || ball->y + ball->radius >= field->height) {
        // Reverse the vertical direction of the ball and adjust position to stay within bounds
        ball->ySpeed *= -1;
        ball->y = fmax(ball->radius, fmin(ball->y, field->height - ball->radius));
    }
}










