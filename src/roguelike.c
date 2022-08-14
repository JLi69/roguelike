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
		
		if(x <= -16.0f * TILE_SIZE || x >= 16.0f * TILE_SIZE 
			|| y <= -10.0f * TILE_SIZE || y >= 10.0f * TILE_SIZE)
			continue;

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
	glUniform2f(glGetUniformLocation(shader, "uTexOffset"), 
									 0.0f + 1.0f / 16.0f * (float)level->player.frame +
									 (float)level->player.animation * 2.0f * 1.0f / 16.0f,
									 15.0f / 16.0f);	
	glDrawArrays(GL_TRIANGLES, 0, 6);

	getGLErrors();
}

void update(Level *level, clock_t start)
{
	static float totalTime = 0.0f;

	//move the player
	if(checkKey(GLFW_KEY_W))
	{
		level->player.animation = MOVING_UP;	
		level->player.dY = PLAYER_SPEED;
	}
	else if(checkKey(GLFW_KEY_S)) 
	{
		level->player.animation = MOVING_DOWN;	
		level->player.dY = -PLAYER_SPEED;
	}
	else
	{
		if(level->player.dY < 0.0f)
			level->player.animation = IDLE_DOWN;
		if(level->player.dY > 0.0f)
			level->player.animation = IDLE_UP;
		level->player.dY = 0.0f;
	}
	
	if(checkKey(GLFW_KEY_A))
	{
		level->player.animation = MOVING_LEFT;	
		level->player.dX = -PLAYER_SPEED;
	}
	else if(checkKey(GLFW_KEY_D))
	{
		level->player.animation = MOVING_RIGHT;	
		level->player.dX = PLAYER_SPEED;
	}
	else
	{
		if(level->player.dX < 0.0f)
			level->player.animation = IDLE_LEFT;
		if(level->player.dX > 0.0f)
			level->player.animation = IDLE_RIGHT;
		level->player.dX = 0.0f;
	}

	static clock_t end;
	static float timePassed = 0.0f;	

	level->player.x += level->player.dX * timePassed;
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
				float diffX;
				if(level->player.x < i)		
					diffX = (level->player.x + 0.5f) - (i - 0.5f); 
				else if(level->player.x > i)
					diffX = (level->player.x - 0.5f) - (i + 0.5f);

				if(level->tiles[index] == WALL)
					level->player.x -= diffX;	
			}
		}
	}

	level->player.y += level->player.dY * timePassed;	
	
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
				float diffY;
				if(level->player.y < j)	
					diffY = (level->player.y + 0.5f) - (j - 0.5f);
				else if(level->player.y > j)
					diffY = (level->player.y - 0.5f) - (j + 0.5f);

				if(level->tiles[index] == WALL)
					level->player.y -= diffY;	
			}
		}
	}

	end = clock();
	while((float)(end - start) / (float)CLOCKS_PER_SEC <= 1.0f / MAX_FPS)
		end = clock();
	timePassed = (float)(end - start) / (float)CLOCKS_PER_SEC;
	updateAnimationFrame(&level->player, totalTime);

	totalTime += timePassed;
}
