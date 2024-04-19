#include "view.h"

void renderField(SDL_Renderer *renderer, SDL_Texture *fieldTexture,int windowWidth, int windowHeight) {
    // Define the new size of the field
    int newWidth = windowWidth * 1; // 100% of the window width
    int newHeight = windowHeight * 1; // 100% of the window height

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
    drawBall(renderer, ball->x, ball->y, ball->radius*1);


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

// Function to render text
void renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color, int x, int y) {
    // Render text
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // Get text dimensions
    int textWidth, textHeight;
    SDL_QueryTexture(texture, NULL, NULL, &textWidth, &textHeight);

    // Position the text
    SDL_Rect textRect = {x, y, textWidth, textHeight};

    // Render the text on the screen
    SDL_RenderCopy(renderer, texture, NULL, &textRect);

    // Clean up
    SDL_DestroyTexture(texture);
}

// Render the timer text
void renderTimer(SDL_Renderer* renderer, TTF_Font* font, Timer* timer, int windowWidth) {
    // Convert to Minutes and seconds
    int totalSeconds = timer->currentTime;
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    // MM:SS Formatting
    char text[20];
    sprintf(text, "%02d:%02d", minutes, seconds);

    // White color
    SDL_Color color = {255, 255, 255};

    // Position the timer text

    int timerX = 170; // Center timer horizontally
    int timerY = 15; // Move down timer

    // Render the text
    renderText(renderer, font, text, color, timerX, timerY);
}

// Render the score text
void renderScore(SDL_Renderer* renderer, TTF_Font* font, Score score, int windowWidth, int windowHeight) {
    // Score formatting
    char text[20];
    sprintf(text, "%02d:%02d", score.team1Score, score.team2Score);

    // White color
    SDL_Color color = {255, 255, 255};

    // Calculate position for the score text
    int textWidth, textHeight;
    TTF_SizeText(font, text, &textWidth, &textHeight);
    int scoreX = (windowWidth - textWidth) / 2; // Centered horizontally
    int scoreY = 15; // Top-aligned

    // Render the text
    renderText(renderer, font, text, color, scoreX, scoreY);
}
