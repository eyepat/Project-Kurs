#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define SPEED 300
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400
#define PLAYER_RADIUS 15 // Storlek på spelarens cirkel

// Funktion för att rita en fylld cirkel
void drawFilledCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // horisontell avstånd från centrum
            int dy = radius - h; // vertikal avstånd från centrum
            if ((dx*dx + dy*dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}

int main(int argv, char** args){
    if(SDL_Init(SDL_INIT_VIDEO)!=0){
        printf("Error: %s\n",SDL_GetError());
        return 1;
    }

    SDL_Window* pWindow = SDL_CreateWindow("Fotbollsspel",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WINDOW_WIDTH,WINDOW_HEIGHT,0);
    if(!pWindow){
        printf("Error: %s\n",SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_Renderer *pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if(!pRenderer){
        printf("Error: %s\n",SDL_GetError());
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;    
    }

    // Ladda fotbollsplansbild
    SDL_Surface *pSurface = IMG_Load("resources/football_field.png");
    if(!pSurface){
        printf("Error: %s\n",SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;    
    }
    SDL_Texture *pFieldTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
    SDL_FreeSurface(pSurface);
    if(!pFieldTexture){
        printf("Error: %s\n",SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;    
    }

    float playerX = WINDOW_WIDTH / 2;
    float playerY = WINDOW_HEIGHT / 2;
    float playerVelocityX = 0;
    float playerVelocityY = 0;

    bool closeWindow = false;
    bool up = false, down = false, left = false, right = false;

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
                            up=true;
                            break;
                        case SDL_SCANCODE_A:
                        case SDL_SCANCODE_LEFT:
                            left=true;
                            break;
                        case SDL_SCANCODE_S:
                        case SDL_SCANCODE_DOWN:
                            down=true;
                            break;
                        case SDL_SCANCODE_D:
                        case SDL_SCANCODE_RIGHT:
                            right=true;
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch(event.key.keysym.scancode){
                        case SDL_SCANCODE_W:
                        case SDL_SCANCODE_UP:
                            up=false;
                            break;
                        case SDL_SCANCODE_A:
                        case SDL_SCANCODE_LEFT:
                            left=false;
                            break;
                        case SDL_SCANCODE_S:
                        case SDL_SCANCODE_DOWN:
                            down=false;
                            break;
                        case SDL_SCANCODE_D:
                        case SDL_SCANCODE_RIGHT:
                            right=false;
                            break;
                    }
                    break;
            }
        }

        playerVelocityX = playerVelocityY = 0;
        if(up && !down) playerVelocityY = -SPEED;
        if(down && !up) playerVelocityY = SPEED;
        if(left && !right) playerVelocityX = -SPEED;
        if(right && !left) playerVelocityX = SPEED;

        playerX += playerVelocityX * 0.016f; // Antag 60 FPS, 0.016 ~ 1/60
        playerY += playerVelocityY * 0.016f;

        // Förhindra spelaren från att gå utanför fönstret
        if(playerX < PLAYER_RADIUS) playerX = PLAYER_RADIUS;
        if(playerY < PLAYER_RADIUS) playerY = PLAYER_RADIUS;
        if(playerX > WINDOW_WIDTH - PLAYER_RADIUS) playerX = WINDOW_WIDTH - PLAYER_RADIUS;
        if(playerY > WINDOW_HEIGHT - PLAYER_RADIUS) playerY = WINDOW_HEIGHT - PLAYER_RADIUS;

        // Rensa skärmen
        SDL_RenderClear(pRenderer);

        // Rita fotbollsplanen
        SDL_RenderCopy(pRenderer, pFieldTexture, NULL, NULL);

        // Rita spelaren som en cirkel
        SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255); // Vit färg för spelaren
        drawFilledCircle(pRenderer, (int)playerX, (int)playerY, PLAYER_RADIUS);

        // Visa det som ritats
        SDL_RenderPresent(pRenderer);

        SDL_Delay(16); // Cirka 60 FPS
    }

    SDL_DestroyTexture(pFieldTexture);
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();
    return 0;
}
