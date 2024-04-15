#include "view.h"

void renderField(SDL_Renderer *renderer, SDL_Texture *fieldTexture,int windowWidth, int windowHeight) {
    // Define the new size of the field
    int newWidth = windowWidth * 0.95; // 90% of the window width
    int newHeight = windowHeight * 0.95; // 90% of the window height

    // Calculate the position to center the field
    int posX = (windowWidth - newWidth) / 2;
    int posY = (windowHeight - newHeight) / 2;

    SDL_Rect fieldRect = {posX, posY, newWidth, newHeight};

    // Render the playing field within the defined rectangle
    SDL_RenderCopy(renderer, fieldTexture, NULL, &fieldRect);
}


void renderPlayer(SDL_Renderer *renderer, const Entity *player) {
    // Render the player entity
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    drawDetailedCircle(renderer, player->x, player->y, player->radius*1.2, 3);

}

void renderBall(SDL_Renderer *renderer, const Entity *ball) {
    // Render the ball entity
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    drawBall(renderer, ball->x, ball->y, ball->radius*2);


}

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

