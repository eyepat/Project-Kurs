#ifndef VIEW_H
#define VIEW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "model.h"

// Function declarations for rendering game elements
void renderField(SDL_Renderer *renderer, SDL_Texture *fieldTexture,int windowWidth, int windowHeight);
void renderPlayers(SDL_Renderer *renderer, const GameState *gameState);
void renderBall(SDL_Renderer *renderer, const Entity *ball);
void drawDetailedCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, int outlineThickness, int colorData[4]);
void drawBall(SDL_Renderer* renderer, int x, int y, int radius);
void renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color, int x, int y);
void renderTimer(SDL_Renderer* renderer, TTF_Font* font, const Timer* timer, int windowWidth);
void renderScore(SDL_Renderer* renderer, TTF_Font* font, Score *score, int windowWidth, int windowHeight);
void renderGoals(SDL_Renderer *renderer, const Field *field);
void modifyPlayerColors(int red, int blue, int green, int opacity, int playerColorData[4]);
void renderButton(SDL_Renderer* renderer, Button* button);
void drawMenu(SDL_Renderer* renderer, TTF_Font* font, MenuState* menuState, int windowWidth, int windowHeight, int *portPointer, char hostIP[20], int *isServerPointer);
void renderTextMenu(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, int w, int h);
void handleUserInput(SDL_Renderer* renderer, TTF_Font* font, MenuState* menuState, int* portPointer, const char* prompt, int windowWidth);

#endif /* VIEW_H */
