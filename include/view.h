#ifndef VIEW_H
#define VIEW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "model.h"

// Function declarations for rendering game elements
void renderField(SDL_Renderer *renderer, SDL_Texture *fieldTexture,int windowWidth, int windowHeight);
void renderPlayer(SDL_Renderer *renderer, const Entity *player);
void renderBall(SDL_Renderer *renderer, const Entity *ball);
void drawDetailedCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, int outlineThickness);
void drawBall(SDL_Renderer* renderer, int x, int y, int radius);
void renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color, int x, int y);
void renderTimer(SDL_Renderer* renderer, TTF_Font* font, Timer* timer, int windowWidth);
void renderScore(SDL_Renderer* renderer, TTF_Font* font, Score score, int windowWidth, int windowHeight);


#endif /* VIEW_H */
