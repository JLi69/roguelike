#include "roguelike.h"
#include <stdio.h>
#include <glad/glad.h>
#include <stdlib.h>
#include <stdio.h>
#include "gl-func.h"
#include "event.h"
#include <math.h>

static GLBufferObj* square;
static unsigned int shader;
static unsigned int tex;
static unsigned int tiles;
static unsigned int sprites;

GLFWwindow* initWindow(void)
{
	//Attempt to initialize glfw
	if(!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW!\n");
		return NULL;
	}

	//Attept to initialize the window
	GLFWwindow* win = glfwCreateWindow(960, 540, "Roguelike", NULL, NULL);
	//Failed to create window
	if(!win)
	{
		fprintf(stderr, "Failed to create window!\n");
		glfwTerminate();	
		return NULL;
	}
	glfwMakeContextCurrent(win);
	
	//Attempt to initialize glad
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		fprintf(stderr, "Failed to load glad!\n");	
		glfwTerminate();
		return NULL;
	}

	//Set up key input
	glfwSetKeyCallback(win, onKeyPress);

	return win;
}

void init(void)
{
	//Square vertex buffer	
	square = createSquare();
	//Shaders
	shader = createShaderProgram("res/shaders/vert.glsl", "res/shaders/texture-frag.glsl");	
	//Textures	
	tex = loadTexture("res/textures/test.png");
	tiles = loadTexture("res/textures/tiles.png");
	sprites = loadTexture("res/textures/sprites.png");
	getGLErrors();
}

void terminate(void)
{
	free(square);
}

void display(Level *level)
{	
	bindGLObj(square);
	glUseProgram(shader);
	glUniform2f(glGetUniformLocation(shader, "uScale"), TILE_SIZE, TILE_SIZE);	
	glUniform1f(glGetUniformLocation(shader, "uTexSize"), 256.0f);	
	glUniform1f(glGetUniformLocation(shader, "uTexFrac"), 1.0f / 16.0f);	
	activateTexture(tiles, GL_TEXTURE0);

	//Clear the screen
	glClear(GL_COLOR_BUFFER_BIT);	

	//Draw the level
	for(int i = 0; i < level->width * level->height; i++)
	{
		float x = (float)(i % level->width) * TILE_SIZE - TILE_SIZE * level->player.x,
			  y = (float)(i / level->width) * TILE_SIZE - TILE_SIZE * level->player.y;
		glUniform2f(glGetUniformLocation(shader, "uOffset"), x, y);	
	
		//Draw different textures for different tiles
		switch(level->tiles[i])
		{
		case EMPTY:
			glUniform2f(glGetUniformLocation(shader, "uTexOffset"), 0.0f, 15.0f / 16.0f);
			break;
		case FLOOR:
			glUniform2f(glGetUniformLocation(shader, "uTexOffset"), 1.0f / 16.0f, 15.0f / 16.0f);	
			break;
		case WALL:
			glUniform2f(glGetUniformLocation(shader, "uTexOffset"), 
						2.0f / 16.0f + (float)(level->randVals[i] % 6) * 1.0f / 16.0f,
						15.0f / 16.0f);	
			break;
		}

		glDrawArrays(GL_TRIANGLES, 0, 6);	
	}

	//Draw the player	
	activateTexture(sprites, GL_TEXTURE0);	
	glUniform2f(glGetUniformLocation(shader, "uOffset"), 0.0f, 0.0f);
	glUniform2f(glGetUniformLocation(shader, "uTexOffset"), 0.0f, 15.0f / 16.0f);	
	glDrawArrays(GL_TRIANGLES, 0, 6);

	getGLErrors();
}

void update(Level *level)
{
	//Check the player's movement	
	if(checkKey(GLFW_KEY_W))
		level->player.dY = 0.12f;
	else if(checkKey(GLFW_KEY_S))
		level->player.dY = -0.12f;
	else
		level->player.dY = 0.0f;
	
	if(checkKey(GLFW_KEY_A))
		level->player.dX = -0.12f;	
	else if(checkKey(GLFW_KEY_D))
		level->player.dX = 0.12f;
	else
		level->player.dX = 0.0f;

	level->player.x += level->player.dX;
	//Player collision
	float playerGridX = floorf(level->player.x),
		  playerGridY = floorf(level->player.y);
	for(float i = playerGridX - 1; i <= playerGridX + 1; i++)
	{	
		for(float j = playerGridY - 1; j <= playerGridY + 1; j++)
		{
			//Check if the player is colliding with it
			struct Sprite tempTile = createSprite(i, j, 1.0f, 1.0f);
			if(colliding(level->player, tempTile))
			{	
				//Check if there is a tile there
				int index = (int)i + (int)j * level->width;
				if(level->tiles[index] == WALL)
					level->player.x -= level->player.dX;	
			}
		}
	}
	level->player.y += level->player.dY;	
	playerGridX = floorf(level->player.x);
	playerGridY = floorf(level->player.y);	
	for(float i = playerGridX - 1; i <= playerGridX + 1; i++)
	{	
		for(float j = playerGridY - 1; j <= playerGridY + 1; j++)
		{
			//Check if the player is colliding with it
			struct Sprite tempTile = createSprite(i, j, 1.0f, 1.0f);
			if(colliding(level->player, tempTile))
			{	
				//Check if there is a tile there
				int index = (int)i + (int)j * level->width;
				if(level->tiles[index] == WALL)
					level->player.y -= level->player.dY;	
			}
		}
	}
}
