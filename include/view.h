#ifndef VIEW_H
#define VIEW_H

#include <SDL2/SDL.h>
#include "model.h"

// Function declarations for rendering game elements
void renderField(SDL_Renderer *renderer, SDL_Texture *fieldTexture);
void renderPlayer(SDL_Renderer *renderer, const Entity *player);
void renderBall(SDL_Renderer *renderer, const Entity *ball);
void drawDetailedCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, int outlineThickness);
void drawBall(SDL_Renderer* renderer, int x, int y, int radius);

#endif /* VIEW_H */
