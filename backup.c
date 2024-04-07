#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h> //text rendering
#define SPEED 500

void drawDetailedCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, int outlineThickness);
void drawBall(SDL_Renderer* renderer, int x, int y, int radius);

int main(int argv, char** args) {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0){
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }
    //for text rendering dvs scoor and time
    if (TTF_Init() == -1) {
        printf("Error initializing SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0) {
        printf("Error getting display mode: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window* pWindow = SDL_CreateWindow("Fotbollsspel",
                                           SDL_WINDOWPOS_UNDEFINED,
                                           SDL_WINDOWPOS_UNDEFINED,
                                           displayMode.w,
                                           displayMode.h,
                                           SDL_WINDOW_FULLSCREEN_DESKTOP);
    if(!pWindow){
        printf("Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_Renderer *pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!pRenderer){
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;    
    }

    SDL_Surface *pSurface = IMG_Load("resources/football_field.png");
    if(!pSurface){
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;    
    }
    SDL_Texture *pFieldTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
    SDL_FreeSurface(pSurface);
    if(!pFieldTexture){
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;    
    }

    int PLAYER_RADIUS = displayMode.w / 128;
    int outlineThickness = 3;
    
    int ballRadius = 20;
    int ballX = displayMode.w / 4;
    int ballY = displayMode.h / 2;
    float ballVelocityX = 0;
    float ballVelocityY = 0;

    int fieldMargin = 100; 
    SDL_Rect fieldRect = {fieldMargin, fieldMargin, displayMode.w - 2 * fieldMargin, displayMode.h - 2 * fieldMargin};

    float playerX = fieldRect.x + fieldRect.w / 2.0f;
    float playerY = fieldRect.y + fieldRect.h / 2.0f;
    float playerVelocityX = 0;
    float playerVelocityY = 0;

    bool closeWindow = false;
    bool up = false, down = false, left = false, right = false;
    int fieldRight = fieldRect.x + fieldRect.w;
    int fieldBottom = fieldRect.y + fieldRect.h;

    // Adjust these values to fine-tune the behavior
    float reboundAcceleration = 0.2; // Increase for faster rebound of the ball
    float friction = 0.98; // Closer to 1 for smoother slowing down
    while(!closeWindow){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    closeWindow = true;
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.scancode){
                        case SDL_SCANCODE_W:
                        case SDL_SCANCODE_UP:
                            up = true;
                            break;
                        case SDL_SCANCODE_A:
                        case SDL_SCANCODE_LEFT:
                            left = true;
                            break;
                        case SDL_SCANCODE_S:
                        case SDL_SCANCODE_DOWN:
                            down = true;
                            break;
                        case SDL_SCANCODE_D:
                        case SDL_SCANCODE_RIGHT:
                            right = true;
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch(event.key.keysym.scancode){
                        case SDL_SCANCODE_W:
                        case SDL_SCANCODE_UP:
                            up = false;
                            break;
                        case SDL_SCANCODE_A:
                        case SDL_SCANCODE_LEFT:
                            left = false;
                            break;
                        case SDL_SCANCODE_S:
                        case SDL_SCANCODE_DOWN:
                            down = false;
                            break;
                        case SDL_SCANCODE_D:
                        case SDL_SCANCODE_RIGHT:
                            right = false;
                            break;
                    }
                    break;
            }
        }

        // Clear the screen with black
        SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
        SDL_RenderClear(pRenderer);

        // Game logic updates
        playerVelocityX = playerVelocityY = 0;
        if(up && !down) playerVelocityY = -SPEED;
        if(down && !up) playerVelocityY = SPEED;
        if(left && !right) playerVelocityX = -SPEED;
        if(right && !left) playerVelocityX = SPEED;
        playerX += playerVelocityX * 0.016f; 
        playerY += playerVelocityY * 0.016f;


        // Ball kicking logic
        float distance = sqrt((playerX - ballX) * (playerX - ballX) + (playerY - ballY) * (playerY - ballY));
       if (distance < PLAYER_RADIUS + ballRadius) {
            // Apply initial kick velocity
            ballVelocityX = (ballX - playerX) * 0.5; // Adjust multiplier to change kick strength
            ballVelocityY = (ballY - playerY) * 0.5;
        }
        // Friction for smoother slowing down
        ballVelocityX *= friction;
        ballVelocityY *= friction;

        // Update ball position
        ballX += ballVelocityX;
        ballY += ballVelocityY;

        // Stop the ball completely if it's moving very slowly to avoid endless sliding
        if (fabs(ballVelocityX) < 0.1) ballVelocityX = 0;
        if (fabs(ballVelocityY) < 0.1) ballVelocityY = 0;

          // Ball physics and collision detection
        if (ballX - ballRadius < fieldRect.x) {
            ballX = fieldRect.x + ballRadius;
            ballVelocityX *= -reboundAcceleration; // Faster rebound
        }
        if (ballX + ballRadius > fieldRight) {
            ballX = fieldRight - ballRadius;
            ballVelocityX *= -reboundAcceleration; 
        }
        if (ballY - ballRadius < fieldRect.y) {
            ballY = fieldRect.y + ballRadius;
            ballVelocityY *= -reboundAcceleration; 
        }
        if (ballY + ballRadius > fieldBottom) {
            ballY = fieldBottom - ballRadius;
            ballVelocityY *= -reboundAcceleration; 
        }
        // Update ball position
        ballX += ballVelocityX;
        ballY += ballVelocityY;
        // Ensure ball does not leave the field
        int fieldRight = fieldRect.x + fieldRect.w;
        int fieldBottom = fieldRect.y + fieldRect.h;
        if (ballX - ballRadius < fieldRect.x) {
            ballX = fieldRect.x + ballRadius;
            ballVelocityX = -ballVelocityX * 0.8; // Add some damping effect
        }
        if (ballX + ballRadius > fieldRight) {
            ballX = fieldRight - ballRadius;
            ballVelocityX = -ballVelocityX * 0.8;
        }
        if (ballY - ballRadius < fieldRect.y) {
            ballY = fieldRect.y + ballRadius;
            ballVelocityY = -ballVelocityY * 0.8;
        }
        if (ballY + ballRadius > fieldBottom) {
            ballY = fieldBottom - ballRadius;
            ballVelocityY = -ballVelocityY * 0.8;
        }
        
        // Draw game elements

        if(playerX < fieldRect.x + PLAYER_RADIUS) playerX = fieldRect.x + PLAYER_RADIUS;
        if(playerY < fieldRect.y + PLAYER_RADIUS) playerY = fieldRect.y + PLAYER_RADIUS;
        if(playerX > fieldRect.x + fieldRect.w - PLAYER_RADIUS) playerX = fieldRect.x + fieldRect.w - PLAYER_RADIUS;
        if(playerY > fieldRect.y + fieldRect.h - PLAYER_RADIUS) playerY = fieldRect.y + fieldRect.h - PLAYER_RADIUS;

        // Draw football field
        SDL_RenderCopy(pRenderer, pFieldTexture, NULL, &fieldRect);
        // Draw player
        drawDetailedCircle(pRenderer, (int)playerX, (int)playerY, PLAYER_RADIUS, outlineThickness);
        // Draw the ball
        drawBall(pRenderer, ballX, ballY, ballRadius);
        // Update the screen
        SDL_RenderPresent(pRenderer);

        SDL_Delay(16);
    }

    SDL_DestroyTexture(pFieldTexture);
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
    // Funktion för att rita en detaljerad cirkel
    void drawDetailedCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, int outlineThickness) {
        // Outline color
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black for the outline
        for (int w = -radius - outlineThickness; w <= radius + outlineThickness; w++) {
            for (int h = -radius - outlineThickness; h <= radius + outlineThickness; h++) {
                int distanceSquared = w * w + h * h;
                if (distanceSquared >= (radius * radius) && distanceSquared <= (radius + outlineThickness) * (radius + outlineThickness)) {
                    SDL_RenderDrawPoint(renderer, centerX + w, centerY + h);
                }
            }
        }

        // Player color RGB dvs changing of this colors bettween 255 and 0 make a different collor 
        int playerR = 255; // Red 
        int playerG = 255;   // Green
        int playerB = 0;   // Blue 
        int playerA = 0; // Alpha component of the player's color (255 is fully opaque)

        // Gradient fill
        for (int w = -radius; w <= radius; w++) {
            for (int h = -radius; h <= radius; h++) {
                if ((w * w + h * h) <= (radius * radius)) {
                    int distance = sqrt(w * w + h * h);
                    int alpha = 255 - (255 * distance / radius);
                    SDL_SetRenderDrawColor(renderer, playerR, playerG, playerB, alpha); // Color for the player
                    SDL_RenderDrawPoint(renderer, centerX + w, centerY + h);
                }
            }
        }
}
void drawBall(SDL_Renderer* renderer, int x, int y, int radius) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0); // Color for the ball
    for (int w = -radius; w < radius; w++) {
        for (int h = -radius; h < radius; h++) {
            if (w * w + h * h < radius * radius) {
                SDL_RenderDrawPoint(renderer, x + w, y + h);
            }
        }
    }
}
