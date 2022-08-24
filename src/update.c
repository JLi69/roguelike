#include "roguelike.h"
#include "event.h"
#include <glad/glad.h>
#include <math.h>
#include <stdlib.h>

#include <stdio.h>

static int* stepsToGoal = NULL;

void update(Level *level, struct timeval start)
{
	static float totalTime = 0.0f;
	static float damageCooldown = 0.0f;

	//Allocate memory for the "step array"
	if(stepsToGoal == NULL)
	{
		stepsToGoal = (int*)malloc(sizeof(int) * level->width * level->height); 	
		bfs(stepsToGoal, *level);	
	}

	//move the player
	if(checkKey(GLFW_KEY_W))
	{
		level->player.spr.animation = MOVING_UP;	
		level->player.spr.dY = PLAYER_SPEED;
	}
	else if(checkKey(GLFW_KEY_S)) 
	{
		level->player.spr.animation = MOVING_DOWN;	
		level->player.spr.dY = -PLAYER_SPEED;
	}
	else
	{
		if(level->player.spr.dY < 0.0f)
			level->player.spr.animation = IDLE_DOWN;
		if(level->player.spr.dY > 0.0f)
			level->player.spr.animation = IDLE_UP;
		level->player.spr.dY = 0.0f;
	}
	
	if(checkKey(GLFW_KEY_A))
	{
		level->player.spr.animation = MOVING_LEFT;	
		level->player.spr.dX = -PLAYER_SPEED;
	}
	else if(checkKey(GLFW_KEY_D))
	{
		level->player.spr.animation = MOVING_RIGHT;	
		level->player.spr.dX = PLAYER_SPEED;
	}
	else
	{
		if(level->player.spr.dX < 0.0f)
			level->player.spr.animation = IDLE_LEFT;
		if(level->player.spr.dX > 0.0f)
			level->player.spr.animation = IDLE_RIGHT;
		level->player.spr.dX = 0.0f;
	}

	//Do a bfs if the player moved
	if(level->player.spr.dX != 0.0f || level->player.spr.dY != 0.0f)
	{
		bfs(stepsToGoal, *level);	
		//DEBUG DELETE LATER		
		/*for(int i = 0; i < level->width * level->height; i++)
		{
			if(stepsToGoal[i] == -1)
				printf(" - ");
			else
				printf(" %d ", stepsToGoal[i]);
			if(i % level->width == level->width - 1)
				printf("\n");
		}
		exit(0);*/
	}

	//Enter = interact key
	int interacting = 0;
	if(checkKey(GLFW_KEY_ENTER))
		interacting = 1;

	static struct timeval end;
	static float timePassed = 0.0f;	

	level->player.spr.x += level->player.spr.dX * timePassed;
	//Player collision
	float playerGridX = floorf(level->player.spr.x),
		  playerGridY = floorf(level->player.spr.y);
	for(float i = playerGridX - 1; i <= playerGridX + 1; i++)
	{	
		for(float j = playerGridY - 1; j <= playerGridY + 1; j++)
		{
			//Check if the player is colliding with it
			struct Sprite tempTile = createSprite(i, j, 1.0f, 1.0f);
			if(colliding(level->player.spr, tempTile))
			{	
				//Check if there is a tile there
				int index = (int)i + (int)j * level->width;
				float diffX;
				if(level->player.spr.x < i)		
					diffX = (level->player.spr.x + 0.5f) - (i - 0.5f); 
				else if(level->player.spr.x > i)
					diffX = (level->player.spr.x - 0.5f) - (i + 0.5f);

				if(level->tiles[index] == WALL)
					level->player.spr.x -= diffX;

				//Traps
				if(level->tiles[index] == SPIKE_TRAP_INACTIVE)	
					level->tiles[index] = SPIKE_TRAP_ACTIVE;
				else if(level->tiles[index] == SPIKE_TRAP_ACTIVE && damageCooldown <= 0.0f)
				{
					damageCooldown = 0.5f;	
					level->player.health--;
				}

				//Interact with the world
				if(interacting)
				{
					switch(level->tiles[index])
					{
					case CHEST:
						level->tiles[index] = OPEN_CHEST;	
						break;
					case EXIT:		
						level->state = NEXT_LEVEL;	
						break;
					case CHEST_MONSTER:
						addEnemy(level, createEnemy(createSprite(i, j, 1.0f, 1.0f), OPEN_CHEST_MONSTER));
						level->tiles[index] = FLOOR;
						damageCooldown = 0.5f;
						level->player.health--;
						break;
					default:
						break;
					}
				}
			}
		}
	}

	//Flash red when the player gets damaged
	if(damageCooldown >= 0.0f)
	{
		useProgram(getShader(DEFAULT_SHADER));	
		glUniform1f(getUniform(DEFAULT_UNIFORM_DAMAGE_COOLDOWN), damageCooldown);
		useProgram(getShader(BACKGROUND_SHADER));
		glUniform1f(getUniform(BACKGROUND_UNIFORM_DAMAGE_COOLDOWN), damageCooldown);	
	}
	//Check if the player died
	if(level->player.health <= 0)
		level->state = DEAD;

	level->player.spr.y += level->player.spr.dY * timePassed;	
	
	playerGridX = floorf(level->player.spr.x);
	playerGridY = floorf(level->player.spr.y);	
	for(float i = playerGridX - 1; i <= playerGridX + 1; i++)
	{	
		for(float j = playerGridY - 1; j <= playerGridY + 1; j++)
		{
			//Check if the player is colliding with it
			struct Sprite tempTile = createSprite(i, j, 1.0f, 1.0f);
			if(colliding(level->player.spr, tempTile))
			{	
				//Check if there is a tile there
				int index = (int)i + (int)j * level->width;
				float diffY;
				if(level->player.spr.y < j)	
					diffY = (level->player.spr.y + 0.5f) - (j - 0.5f);
				else if(level->player.spr.y > j)
					diffY = (level->player.spr.y - 0.5f) - (j + 0.5f);

				if(level->tiles[index] == WALL)
					level->player.spr.y -= diffY;	
			}
		}
	}	
	
	updateAnimationFrame(&level->player.spr, totalTime);

	for(int i = 0; i < level->enemyCount; i++)
	{
		updateAnimationFrame(&level->enemies[i].spr, totalTime);
		//Check if the player is colliding with any of them and damage the player if it is
		if(colliding(level->player.spr, level->enemies[i].spr) && damageCooldown <= 0.0f)
		{
			level->player.health -= getEnemyDamageVal(level->enemies[i].type);
			damageCooldown = 0.5f;	
		}

		//Update the enemy position
		switch(level->enemies[i].type)
		{
		case SLIME:		updateSlime(&level->enemies[i], stepsToGoal, level->width, level->height, level->enemies, level->enemyCount, timePassed);		break;
		default:		break;
		}
	}

	if(level->state != PLAYING)
	{
		free(stepsToGoal);
		stepsToGoal = NULL;
	}

	totalTime += timePassed;
	damageCooldown -= timePassed;

	//Measure the amount of time that passed
	gettimeofday(&end, 0);
	timePassed = end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) * 1e-6;
}
