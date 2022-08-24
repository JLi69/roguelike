#ifndef INCLUDE_ROGUELIKE
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <sys/time.h>
#include "level.h"
#include "gl-func.h"

#define MAX_FPS 120.0f

#define TILE_SIZE 48.0f
#define HUD_SIZE 16.0f
#define PLAYER_SPEED 8.0f

//Buffers
#define BUFFER_COUNT 1
#define SQUARE 0

//Shaders
#define SHADER_COUNT 2
#define DEFAULT_SHADER 0
#define BACKGROUND_SHADER 1
//Uniforms
#define UNIFORM_COUNT 16
enum Uniform
{ 
	DEFAULT_UNIFORM_OFFSET, 
	DEFAULT_UNIFORM_SCALE,
	DEFAULT_UNIFORM_SCREEN_DIMENSIONS, 
	DEFAULT_UNIFORM_DAMAGE_COOLDOWN,
	DEFAULT_UNIFORM_TEX_SIZE,
	DEFAULT_UNIFORM_TEX_FRAC,
	DEFAULT_UNIFORM_TEX_OFFSET,

	BACKGROUND_UNIFORM_SCALE,
	BACKGROUND_UNIFORM_SCREEN_DIMENSIONS,
	BACKGROUND_UNIFORM_DAMAGE_COOLDOWN,
	BACKGROUND_UNIFORM_TEX_SIZE,
	BACKGROUND_UNIFORM_TEX_FRAC,
	BACKGROUND_UNIFORM_TEX_OFFSET,
};

//Textures
#define TEXTURE_COUNT 3
#define TILE_TEXTURE_MAP 0
#define SPRITE_TEXTURE_MAP 1
#define ICON_TEXTURE_MAP 2

//Initialize glfw and glad and also create the game window
GLFWwindow* initWindow(void);
//Create vertex buffers, create shaders, load textures
void init(void);
//Deallocate memory when game ends
void terminate(void);
//Display the level and draw the sprites
void display(Level *level);
//Update the sprites on a level
void update(Level *level, struct timeval start);

unsigned int getShader(unsigned int shader);
unsigned int getTexture(unsigned int texture);
int getUniform(unsigned int uniform);
GLBufferObj* getBuffer(unsigned int buffer);

#endif

#define INCLUDE_ROGUELIKE
