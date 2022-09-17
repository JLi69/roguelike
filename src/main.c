#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>

#include "gl-func.h"
#include "structs.h"
#include "level.h"
#include "roguelike.h"
#include <sys/time.h>
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
 *			- Rooms are bit boring, fill them up with something?
 *				- Room Types?
 *					- Slime Room
 *					- Spider Room
 *					- Graveyard
 *					- Garden
 *					- Prison
 *		- Generate enemy locations
 *		- Generate power up locations
 *		- Generate loot locations 
 *		- Levels get bigger and have more enemies the farther you adventure [DONE]
 *			- 10 total levels
 *  - Enemies	
 *		- Slime (weakest) [DONE]
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
	int levelNum = 0; 
	unsigned int seed = time(NULL);
	Level* level = genLevel(seed, levelNum);		

	GLFWwindow* win = initWindow();
	if(!win) return -1;	
	init();

	//Main loop
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	struct timeval frameStart;
	gettimeofday(&frameStart, 0);
	while(!glfwWindowShouldClose(win))
	{
		display(level);
		//GLFW stuff
		glfwSwapBuffers(win);
		glfwPollEvents();

		update(level, frameStart);
		
		//Advance to the next level
		if(level->state == NEXT_LEVEL)
		{
			struct Player tempPlayer = level->player; //Keep the player's stats	
			destroyLevel(level);
			level = genLevel(0, ++levelNum);
			tempPlayer.spr = level->player.spr;
			level->player = tempPlayer;
		}
		//Player died
		else if(level->state == DEAD)
		{
			destroyLevel(level);
			levelNum = 0;
			seed += 2;
			level = genLevel(seed, levelNum);	
		}

		gettimeofday(&frameStart, 0);
	}
	//Deallocate memory when program ends
	glfwTerminate();	
	destroyLevel(level);
	terminate();
}
