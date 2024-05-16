#include "view.h"
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
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

void renderPlayers(SDL_Renderer *renderer, const GameState *gameState) {
    // Iterate over each player in the game state
    
    // Render the player entity
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    
    for (int i = 0; i < gameState->numPlayers; i++)
    {
        drawDetailedCircle(renderer, gameState->players[i].x, gameState->players[i].y , 14, 3, (int *) gameState->players[i].colorData);

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

    int timerX = (windowWidth / 2 ) - 65; // Center timer horizontally
    int timerY = 15; // Move down timer

    // Render the text
    renderText(renderer, font, text, color, timerX, timerY);
}

// Render the score text
void renderScore(SDL_Renderer* renderer, TTF_Font* font, Score *score, int windowWidth, int windowHeight) {
    char text[20];
    sprintf(text, "%d : %d", score->team1Score, score->team2Score);  // Format: "Team1Score : Team2Score"

    SDL_Color color = {255, 255, 255};  // vit färg för poängtexten
    renderText(renderer, font, text, color, 185, 15);  // Justera positionen vid behov
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


void drawMenu(SDL_Renderer* renderer, TTF_Font* font, MenuState* menuState, int windowWidth, int windowHeight, int *portPointer, char hostIP[20], int *isServerPointer) {
        
    menuState->hostButton.bounds.x = windowWidth/2 - 100; 
    menuState->hostButton.bounds.y = 100; 
    menuState->hostButton.bounds.w = 200; // Button width is 200 pixels
    menuState->hostButton.bounds.h = 50;  // Button height is 50 pixels
    
    menuState->joinButton.bounds.x = windowWidth/2 - 100; // Same x as hostButton 
    menuState->joinButton.bounds.y = 200; // 200 pixels from the top, so it's below hostButton
    menuState->joinButton.bounds.w = 200; // Same width as hostButton
    menuState->joinButton.bounds.h = 50;  // Same height as hostButton

    menuState->exitButton.bounds.x = windowWidth/2 - 100; // Same x as other buttons 
    menuState->exitButton.bounds.y = 300; // Below the other buttons
    menuState->exitButton.bounds.w = 200; // Same width as other buttons
    menuState->exitButton.bounds.h = 50;  // Same height as other buttons

    menuState->startButton.bounds.x = windowWidth/2 - 100; 
    menuState->startButton.bounds.y = 300; // startButton position as exit
    menuState->startButton.bounds.w = 200; 
    menuState->startButton.bounds.h = 50;  
        
    menuState->joinHostButton.bounds.x= windowWidth/2 - 100;
    menuState->joinHostButton.bounds.y = 300; // joinHostButton position as exit
    menuState->joinHostButton.bounds.w = 200; 
    menuState->joinHostButton.bounds.h = 50;  

    menuState->onlineButton.bounds.x= windowWidth/2 - 100;
    menuState->onlineButton.bounds.y = 100; // onlineButton position as host
    menuState->onlineButton.bounds.w = 200; 
    menuState->onlineButton.bounds.h = 50;  

    menuState->localButton.bounds.x= windowWidth/2 - 100;
    menuState->localButton.bounds.y = 200; // localButton same position as join
    menuState->localButton.bounds.w = 200; 
    menuState->localButton.bounds.h = 50;  

    menuState->ipInputButton.bounds.x = windowWidth/2 - 100; 
    menuState->ipInputButton.bounds.y = 100; // ipInputButton same position as host
    menuState->ipInputButton.bounds.w = 200; 
    menuState->ipInputButton.bounds.h = 50;  

    strcpy(menuState->ip, "127.0.0.1"); //same position as host
    char displayMessage[30] = "Enter host IP";
        
    SDL_RenderCopy(renderer, menuState->menuBackground, NULL, NULL);
    //menuState->menuState = 0;//start menu, choose to play online or local
    //menuState->menuState = 1;//online menu
    //menuState->menuState = 2;//start local two player game
    //menuState->menuState = 3;//host menu
    //menuState->menuState = 4;//join menu
    //menuState->menuState = 5;//exit game
    //menuState->menuState = 6;//start game
    //menuState->menuState = 7;//join host
    //menuState->menuState = 8;//enter ip
    //startButton position as exit
    //joinHostButton position as exit
    //onlineButton position as host
    //localButton same position as join
    //ipInputButton same position as host

    if (menuState->menuState == 0) { //start menu
        SDL_RenderCopy(renderer, menuState->onlineButton.texture, NULL, &menuState->onlineButton.bounds);
        SDL_RenderCopy(renderer, menuState->localButton.texture, NULL, &menuState->localButton.bounds);
        SDL_RenderPresent(renderer);
    }

    if (menuState->menuState == 1) { //online menu
        SDL_RenderCopy(renderer, menuState->hostButton.texture, NULL, &menuState->hostButton.bounds);
        SDL_RenderCopy(renderer, menuState->joinButton.texture, NULL, &menuState->joinButton.bounds);
        SDL_RenderCopy(renderer, menuState->exitButton.texture, NULL, &menuState->exitButton.bounds);
        SDL_RenderPresent(renderer);
    }

    if (menuState->menuState == 9) // Connect client to host after taking IP and port
    {
        /*SDL_Color color = {255, 255, 255}; 
        SDL_Surface* surface = TTF_RenderText_Solid(font, "Connecting to host...", color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dstrect = {windowWidth/2 - 100, 200, 300, 50}; // Position where you want to show the IP text
        SDL_RenderCopy(renderer, texture, NULL, &dstrect);   */
        SDL_RenderClear(renderer);
        menuState->menuState = 6;
    }

    if (menuState->menuState == 11)
    {
        SDL_Color color = {255, 255, 255}; // White color
        SDL_Surface* surface = TTF_RenderText_Solid(font, "Waiting for players 1/4...", color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dstrect = {windowWidth/2 - 150, 300, 300, 50}; // Position where you want to show the IP text
        SDL_RenderCopy(renderer, menuState->menuBackground, NULL, NULL);
        SDL_RenderCopy(renderer, texture, NULL, &dstrect);
        SDL_RenderPresent(renderer);
    }
    


    if (menuState->menuState == 12)
    {
        SDL_Color color = {255, 255, 255}; // White color
        SDL_Surface* surface = TTF_RenderText_Solid(font, "Waiting for players 2/4...", color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dstrect = {windowWidth/2 - 150, 300, 300, 50}; // Position where you want to show the IP text
        SDL_RenderCopy(renderer, menuState->menuBackground, NULL, NULL);
        SDL_RenderCopy(renderer, texture, NULL, &dstrect);
        SDL_RenderPresent(renderer);
    }

    if (menuState->menuState == 13)
    {
        SDL_Color color = {255, 255, 255}; // White color
        SDL_Surface* surface = TTF_RenderText_Solid(font, "Waiting for players 3/4...", color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dstrect = {windowWidth/2 - 150, 300, 300, 50}; // Position where you want to show the IP text
        SDL_RenderCopy(renderer, menuState->menuBackground, NULL, NULL);
        SDL_RenderCopy(renderer, texture, NULL, &dstrect);
        SDL_RenderPresent(renderer);
    }

    if (menuState->menuState == 14)
    {
        SDL_Color color = {255, 255, 255}; // White color
        SDL_Surface* surface = TTF_RenderText_Solid(font, "Starting game with 4/4 players...", color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dstrect = {windowWidth/2 - 150, 300, 300, 50}; // Position where you want to show the IP text
        SDL_RenderCopy(renderer, menuState->menuBackground, NULL, NULL);
        SDL_RenderCopy(renderer, texture, NULL, &dstrect);
        SDL_RenderPresent(renderer);
    }

    if (menuState->menuState == 15)
    {
        SDL_Color color = {255, 255, 255}; // White color
        SDL_Surface* surface = TTF_RenderText_Solid(font, "Connecting to host...", color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dstrect = {windowWidth/2 - 150, 300, 300, 50}; // Position where you want to show the IP text
        SDL_RenderCopy(renderer, menuState->menuBackground, NULL, NULL);
        SDL_RenderCopy(renderer, texture, NULL, &dstrect);
        SDL_RenderPresent(renderer);
    }

    if (menuState->menuState == 16)
    {
        SDL_Color color = {255, 255, 255}; // White color
        SDL_Surface* surface = TTF_RenderText_Solid(font, "Connected to host and waiting", color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dstrect = {windowWidth/2 - 150, 300, 300, 50}; // Position where you want to show the IP text
        SDL_RenderCopy(renderer, menuState->menuBackground, NULL, NULL);
        SDL_RenderCopy(renderer, texture, NULL, &dstrect);
        SDL_RenderPresent(renderer);
    }
    
    
    
    

    if (menuState->menuState == 10)
    {
        
        *isServerPointer = 1;
        menuState->menuState = 6;

         
    }
        
    if (menuState->menuState == 3) { //host button clicked
        // Show the IP address
        //SDL_RenderCopy(renderer, menuState->startButton.texture, NULL, &menuState->startButton.bounds);
        //strcpy(menuState->ip, "127.0.0.1"); //same position as host

        // Draw the IP address
                 
        SDL_Color color = {255, 255, 255}; // White color
        SDL_Surface* surface = TTF_RenderText_Solid(font, "Enter port", color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dstrect = {windowWidth/2 - 100, 200, 200, 50}; // Position where you want to show the IP text
        SDL_RenderCopy(renderer, texture, NULL, &dstrect);
        int type = 1;
        int position = 0;

        memset(menuState->userInputIp, 0, 20);
        //user ip input while loop
        while (type) { // While type is true
            SDL_Event event;
            while (SDL_PollEvent(&event)) { // Poll for events
                if (event.type == SDL_KEYDOWN) { // If a key was pressed
                    // If the key was a number or a dot
                    if ((event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9) || event.key.keysym.sym == SDLK_PERIOD) {
                        // If position is less than 15
                        if (position < 15) {
                            // Add the character to userInputIp
                            menuState->userInputIp[position] = event.key.keysym.sym;
                            position++;
                        }
                    }
                    // If the backspace key was pressed and position is greater than 0
                    if (event.key.keysym.sym == SDLK_BACKSPACE && position > 0) {
                        position--;
                        // Remove the last character from userInputIp
                        menuState->userInputIp[position] = 0;
                    }
                    // If the return key was pressed
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        // Set type to false 
                        type = false;
                    
                        *portPointer = atoi(menuState->userInputIp);
                        printf("%d ", *portPointer);
                        menuState->menuState = 10;
                        
                    }
                }
            }

            // Render the ipInputButton texture
            SDL_RenderCopy(renderer, menuState->ipInputButton.texture, NULL, &menuState->ipInputButton.bounds);

            SDL_Surface* surface = TTF_RenderText_Solid(font, menuState->userInputIp, color);

            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

            SDL_Rect dstrect = {windowWidth/2 - 100, 100, 200, 50}; // Position of the ip

            // Render the text over the image
            SDL_RenderCopy(renderer, texture, NULL, &dstrect);
            SDL_RenderPresent(renderer); //update screen

            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
            //menuState->menuState = 5;
        }//while type=true



    
    }


        if (menuState->menuState == 8) //Enter host port for connecting to host
    {
        SDL_Color color = {255, 255, 255}; 
        SDL_Surface* surface = TTF_RenderText_Solid(font, "Enter host port", color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dstrect = {windowWidth/2 - 100, 200, 200, 50}; // Position where you want to show the IP text
        SDL_RenderCopy(renderer, texture, NULL, &dstrect);
        int type = true;
        int position = 0;

        memset(menuState->userInputIp, 0, 20);
        //user ip input while loop
        while (type) { // While type is true
            SDL_Event event;
            while (SDL_PollEvent(&event)) { // Poll for events
                if (event.type == SDL_KEYDOWN) { // If a key was pressed
                    // If the key was a number or a dot
                    if ((event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9) || event.key.keysym.sym == SDLK_PERIOD) {
                        // If position is less than 15
                        if (position < 15) {
                            // Add the character to userInputIp
                            menuState->userInputIp[position] = event.key.keysym.sym;
                            position++;
                        }
                    }
                    // If the backspace key was pressed and position is greater than 0
                    if (event.key.keysym.sym == SDLK_BACKSPACE && position > 0) {
                        position--;
                        // Remove the last character from userInputIp
                        menuState->userInputIp[position] = 0;
                    }
                    // If the return key was pressed
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        // Set type to false 
                        type = false;
                        *isServerPointer = 2;
                        *portPointer = atoi(menuState->userInputIp);
                        printf("%d ", *portPointer);
                        menuState->menuState = 9;
                        
                    }
                }
            }

            // Render the ipInputButton texture
            SDL_RenderCopy(renderer, menuState->ipInputButton.texture, NULL, &menuState->ipInputButton.bounds);

            SDL_Surface* surface = TTF_RenderText_Solid(font, menuState->userInputIp, color);

            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

            SDL_Rect dstrect = {windowWidth/2 - 100, 100, 200, 50}; // Position of the ip

            // Render the text over the image
            SDL_RenderCopy(renderer, texture, NULL, &dstrect);
            SDL_RenderPresent(renderer); //update screen

            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
            //menuState->menuState = 5;
        }//while type=true
    }

    if (menuState->menuState == 4) { // If the menu state is 4 (join client menu)

        *isServerPointer = 2;
        int type = true; 
        int position = 0; // Position in userInputIp 
        SDL_Color color = {255, 255, 255}; 

        SDL_Surface* surface = TTF_RenderText_Solid(font, "Enter host IP", color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dstrect = {windowWidth/2 - 100, 200, 200, 50}; // Position where you want to show the IP text
        SDL_RenderCopy(renderer, texture, NULL, &dstrect);
        

        memset(menuState->userInputIp, 0, 20);
        //user ip input while loop
        while (type) { // While type is true
            SDL_Event event;
            while (SDL_PollEvent(&event)) { // Poll for events
                if (event.type == SDL_KEYDOWN) { // If a key was pressed
                    // If the key was a number or a dot
                    if ((event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9) || event.key.keysym.sym == SDLK_PERIOD) {
                        // If position is less than 15
                        if (position < 15) {
                            // Add the character to userInputIp
                            menuState->userInputIp[position] = event.key.keysym.sym;
                            position++;
                        }
                    }
                    // If the backspace key was pressed and position is greater than 0
                    if (event.key.keysym.sym == SDLK_BACKSPACE && position > 0) {
                        position--;
                        // Remove the last character from userInputIp
                        menuState->userInputIp[position] = 0;
                    }
                    // If the return key was pressed
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        // Set type to false 
                        type = false;
                        strcpy(hostIP, menuState->userInputIp);

                        menuState->menuState = 8;
                        
                    }
                }
            }

            // Render the ipInputButton texture
            SDL_RenderCopy(renderer, menuState->ipInputButton.texture, NULL, &menuState->ipInputButton.bounds);

            SDL_Surface* surface = TTF_RenderText_Solid(font, menuState->userInputIp, color);

            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

            SDL_Rect dstrect = {windowWidth/2 - 100, 100, 200, 50}; // Position of the ip

            // Render the text over the image
            SDL_RenderCopy(renderer, texture, NULL, &dstrect);
            SDL_RenderPresent(renderer); //update screen

            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
            //menuState->menuState = 5;
        }//while type=true
    }    

}
