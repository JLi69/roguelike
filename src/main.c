#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>

#include "gl-func.h"
#include "structs.h"
#include "level.h"
#include "roguelike.h"
#include <time.h>

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

int main(void)
{
	//Test level
	Level* level = genLevel(0, 0);		

	GLFWwindow* win = initWindow();
	if(!win) return -1;	
	init();

	//Main loop
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	clock_t frameStart = clock();	
	while(!glfwWindowShouldClose(win))
	{
		display(level);
		//GLFW stuff
		glfwSwapBuffers(win);
		glfwPollEvents();

		update(level, frameStart);

		frameStart = clock();
	}
	//Deallocate memory when program ends
	glfwTerminate();	
	destroyLevel(level);
	terminate();
}
