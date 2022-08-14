#ifndef INCLUDE_ROGUELIKE
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "level.h"

#define TILE_SIZE 48.0f

//Initialize glfw and glad and also create the game window
GLFWwindow* initWindow(void);
//Create vertex buffers, create shaders, load textures
void init(void);
//Deallocate memory when game ends
void terminate(void);
//Display the level and draw the sprites
void display(Level *level);
//Update the sprites on a level
void update(Level *level);

#endif

#define INCLUDE_ROGUELIKE
