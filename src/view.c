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

void renderPlayers(SDL_Renderer *renderer, const Entity players[], int numPlayers) {
    // Iterate over each player in the array
    for (int i = 0; i < numPlayers; i++) {
        // Render the player entity
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        drawDetailedCircle(renderer, players[i].x, players[i].y, players[i].radius * 1.2, 3, players[i].colorData);
    }
}



void renderBall(SDL_Renderer *renderer, const Entity *ball) {
    // Render the ball entity
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    drawBall(renderer, ball->x, ball->y, ball->radius*1);


}

void drawDetailedCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, int outlineThickness, int colorData[4]) {
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
        int playerR = colorData[0]; // Red 
        int playerG = colorData[1];  // Green
        int playerB = colorData[2];  // Blue 
        int playerA = colorData[3]; // Alpha component of the player's color (255 is fully opaque)

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

    int timerX = 350; // Center timer horizontally
    int timerY = 33; // Move down timer

    // Render the text
    renderText(renderer, font, text, color, timerX, timerY);
}

// Render the score text
void renderScore(SDL_Renderer *renderer, TTF_Font *font, Score score, int windowWidth, int windowHeight) {
    char text[20];
    sprintf(text, "%d : %d", score.team1Score, score.team2Score);  // Format: "Team1Score : Team2Score"

    SDL_Color color = {255, 255, 255};  // vit färg för poängtexten
    renderText(renderer, font, text, color, windowWidth / 2 - 50, 50);  // Justera positionen vid behov
}
void renderGoals(SDL_Renderer *renderer, const Field *field) {
    // color for outlin, exempelvis vit
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    int outlineThickness = 2; // Du kan justera tjockleken efter behov

    // Rendera vänstra målet med outline
    SDL_Rect outlineRectLeft = {field->goals[0].box.x - outlineThickness, field->goals[0].box.y - outlineThickness,
                                field->goals[0].box.w + outlineThickness * 2, field->goals[0].box.h + outlineThickness * 2};
    SDL_RenderDrawRect(renderer, &outlineRectLeft);

    // Rendera högra målet med outline
    SDL_Rect outlineRectRight = {field->goals[1].box.x - outlineThickness, field->goals[1].box.y - outlineThickness,
                                 field->goals[1].box.w + outlineThickness * 2, field->goals[1].box.h + outlineThickness * 2};
    SDL_RenderDrawRect(renderer, &outlineRectRight);

    // color for goal
    SDL_SetRenderDrawColor(renderer, 190, 190, 190, 190); // Ändra färgvärden efter behov

    // render left goal
    SDL_RenderFillRect(renderer, &field->goals[0].box);

    // render right goal
    SDL_RenderFillRect(renderer, &field->goals[1].box);
}

void modifyPlayerColors(int red, int blue, int green, int opacity, int playerColorData[4]){
    playerColorData[0] = red;
    playerColorData[1] = blue;
    playerColorData[2] = green;
    playerColorData[3] = opacity; //Genomskinlighet

}


void renderButton(SDL_Renderer* renderer, Button* button) {
    SDL_RenderCopy(renderer, button->texture, NULL, &button->bounds);
}

void drawMenu(SDL_Renderer* renderer, TTF_Font* font, GameState* gameState) {
    SDL_RenderCopy(renderer, gameState->menuBackground, NULL, NULL);
    // Draw the host button
    SDL_RenderCopy(renderer, gameState->hostButton.texture, NULL, &gameState->hostButton.bounds);
    // Draw the join button
    SDL_RenderCopy(renderer, gameState->joinButton.texture, NULL, &gameState->joinButton.bounds);
    // Draw the exit button
    SDL_RenderCopy(renderer, gameState->exitButton.texture, NULL, &gameState->exitButton.bounds);
    // Draw the start button
    if (gameState->menuState == 11) {
        // Show the IP address
        SDL_RenderCopy(renderer, gameState->startButton.texture, NULL, &gameState->startButton.bounds);
        strcpy(gameState->ip, "192.168.1.1"); // Replace this to the real IP 

        //gameState.startButton.bounds.x = 100; // Same x as other buttons for alignment
        //gameState.startButton.bounds.y = 300; // Below the other buttons
        //gameState.startButton.bounds.w = 200; // Same width as other buttons
        //gameState.startButton.bounds.h = 50;  // Same height as other buttons
        //fix ip so that it shows above the start button

        // Draw the IP address
        /*
        SDL_Color color = {255, 255, 255}; // White color
        SDL_Surface* surface = TTF_RenderText_Solid(font, gameState->ip, color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dstrect = {100, 500, surface->w, surface->h}; // Position where you want to show the IP address
        SDL_RenderCopy(renderer, texture, NULL, &dstrect);
        */
    }

}
