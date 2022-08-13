#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
#include <SOIL2/SOIL2.h>

#include "gl-func.h"
#include "structs.h"
#include "level.h"

/*
 * TODO List:
 *  - Be able to draw to the screen using OpenGL 
 *		- Vertex Buffer (square) [DONE]
 *		- Shaders [DONE]
 *			- TODO: compile error handling [DONE]
 *		- Texture Coordinate Buffer (square) + Textures [DONE]
 *  - Level generation
 *		- Generate a maze of hallways [DONE]
 *		- Generate rooms randomly throughout the hallways [DONE]
 *		- Generate enemy locations
 *		- Generate power up locations
 *		- Generate loot locations 
 *		- Levels get bigger and have more enemies the farther you adventure
 *			- 10 total levels
 *  - Enemies	
 *		- Slime (weakest)
 *		- Snake
 *		- Spider
 *		- Ghost
 *		- Demon (strongest)
 *		- Final boss?
 *  - Power ups
 *		- Health potion, Health upgrade
 *		- Speed Potion
 *		- Strength Potion	
 *		- Metal Sword, Admantine Sword, Demon Sword
 *		- Bomb
 *  - Art
 */

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

	return win;
}

int main(void)
{
	//Test level
	Level* level = genLevel(0, 0);			

	GLFWwindow* win = initWindow();
	if(!win) return -1;

	//OpenGL stuff
	GLBufferObj* square = createSquare();
	bindGLObj(square);
	getGLErrors();
	
	unsigned int shader = createShaderProgram("res/shaders/vert.glsl", "res/shaders/texture-frag.glsl");
	glUseProgram(shader);

	unsigned int tex = loadTexture("res/textures/test.png");	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);	
	glUniform2f(glGetUniformLocation(shader, "uScale"), 4.0f / 960.0f, 4.0f / 540.0f);
	
	//Main loop
	while(!glfwWindowShouldClose(win))
	{
		glClear(GL_COLOR_BUFFER_BIT);	

		//Draw the level
		for(int i = 0; i < level->width * level->height; i++)
		{
			float x = (float)(i % level->width) * 8.0f / 960.0f - 0.5f,
				  y = (float)(i / level->width) * 8.0f / 540.0f - 0.5f;
			glUniform2f(glGetUniformLocation(shader, "uOffset"), x, y);
			if(level->tiles[i] == WALL)
				glDrawArrays(GL_TRIANGLES, 0, 6);	
		}	
		getGLErrors();

		//GLFW stuff
		glfwSwapBuffers(win);
		glfwPollEvents();
	}
	//Deallocate memory when program ends
	glfwTerminate();
	free(square);
	destroyLevel(level);
}
