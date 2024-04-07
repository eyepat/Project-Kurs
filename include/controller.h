#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <SDL2/SDL.h>
#include "model.h"

// Function declarations for handling user input and events
void handleEvents(bool *closeWindow, bool *up, bool *down, bool *left, bool *right);

#endif /* CONTROLLER_H */
