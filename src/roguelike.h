#ifndef INCLUDE_ROGUELIKE
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <time.h>
#include "level.h"

#define MAX_FPS 120.0f

#define TILE_SIZE 48.0f
#define PLAYER_SPEED 16.0f

//Initialize glfw and glad and also create the game window
GLFWwindow* initWindow(void);
//Create vertex buffers, create shaders, load textures
void init(void);
//Deallocate memory when game ends
void terminate(void);
//Display the level and draw the sprites
void display(Level *level);
//Update the sprites on a level
void update(Level *level, clock_t start);

#endif

#define INCLUDE_ROGUELIKE
