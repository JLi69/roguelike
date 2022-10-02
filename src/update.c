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
	static float ticker = 0.0f;
	
	static struct timeval end;
	static float timePassed = 0.0f;

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

	//Attack
	if(checkKey(GLFW_KEY_SPACE) && level->player.attackCooldown <= 0.0f)
	{
		//Damage all enemies within range
		for(int i = 0; i < level->enemyCount; i++)
		{
			//Enemy is dead
			if(level->enemies[i].health <= 0)
				continue;

			//Check if the player is facing the enemy's direction
			switch(level->player.spr.animation)
			{
			case IDLE_UP: case MOVING_UP:
				if(level->enemies[i].spr.y < level->player.spr.y + 0.5f)
					continue;
				break;
			case IDLE_DOWN: case MOVING_DOWN:
				if(level->enemies[i].spr.y > level->player.spr.y - 0.5f)
					continue;	
				break;
			case IDLE_RIGHT: case MOVING_RIGHT:
				if(level->enemies[i].spr.x < level->player.spr.x - 0.5f)
					continue;		
				break;
			case IDLE_LEFT: case MOVING_LEFT:
				if(level->enemies[i].spr.x > level->player.spr.x + 0.5f)
					continue;	
				break;
			}

			//Enemy is in range
			if(spriteDist(level->enemies[i].spr, level->player.spr) < 2.0f && level->enemies[i].damageCooldown <= 0.0f)
			{
				level->enemies[i].damageCooldown = 0.5f;
				level->enemies[i].health--;
			}

			//Drop items upon death
			if(level->enemies[i].health <= 0)
			{
				//Drop coins / treasure
				for(int j = 0; j < getEnemyScoreVal(level->enemies[i].type); j++)
				{
					float dist = (float)rand() / (float)RAND_MAX * 0.4f,
						  angle = (float)rand() / (float)RAND_MAX * 3.141592653f;
					addItem(level, createItem(createSprite(level->enemies[i].animationX + dist * cosf(angle), level->enemies[i].animationY + dist * sinf(angle), 0.5f, 0.5f), COIN));
				}
			}
		}

		level->player.attackCooldown = ATTACK_COOLDOWN_LENGTH;
	}

	if(level->player.attackCooldown > 0.0f)
		level->player.attackCooldown -= timePassed;

	//Do a bfs if the player moved
	if(level->player.spr.dX != 0.0f || level->player.spr.dY != 0.0f)
		bfs(stepsToGoal, *level);			

	//Enter = interact key
	int interacting = 0;
	if(checkKey(GLFW_KEY_ENTER))
		interacting = 1;	

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
						level->player.score++;	
						level->tiles[index] = OPEN_CHEST;	
						break;
					case EXIT:		
						level->state = NEXT_LEVEL;	
						break;
					case CHEST_MONSTER:
						level->player.score += 5;
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

	//Update the enemies
	for(int i = 0; i < level->enemyCount; i++)
	{
		//Enemy is dead
		if(level->enemies[i].health <= 0 && level->enemies[i].damageCooldown <= 0.0f)
			continue;
		else if(level->enemies[i].health <= 0 && level->enemies[i].damageCooldown > 0.0f)
		{
			level->enemies[i].damageCooldown -= timePassed * 4.0f; //Enemy turns red upon death and then disappears	
			continue;
		}

		updateAnimationFrame(&level->enemies[i].spr, totalTime);
		//Check if the player is colliding with any of them and damage the player if it is
		if(colliding(level->player.spr, level->enemies[i].spr) && damageCooldown <= 0.0f)
		{
			level->player.health -= getEnemyDamageVal(level->enemies[i].type);
			damageCooldown = 0.5f;	
		}

		//Check if the enemy landed on certain tiles and then have them interact with thos tiles
		if(level->enemies[i].damageCooldown <= 0.0f)
		{
			int index = (int)level->enemies[i].spr.x + (int)level->enemies[i].spr.y * level->width;
			if(index < 0 || index >= level->width * level->height)
				continue;	
			switch(level->tiles[index])
			{
			case SPIKE_TRAP_INACTIVE:	level->tiles[index] = SPIKE_TRAP_ACTIVE;		break;
			case SPIKE_TRAP_ACTIVE:		
				level->enemies[i].health--;
				level->enemies[i].damageCooldown = 0.5f;	
				break;
			default: break;
			} 
		}
		level->enemies[i].damageCooldown -= timePassed;

		//Update the enemy position
		switch(level->enemies[i].type)
		{
		case SLIME:		updateEnemy(&level->enemies[i], stepsToGoal, level->width, level->height, level->enemies, level->enemyCount, timePassed, SLIME_UPDATE_TIME, SLIME_MOVEMENT_TIME, 16);		break;
		case SNAKE:		updateEnemy(&level->enemies[i], stepsToGoal, level->width, level->height, level->enemies, level->enemyCount, timePassed, SNAKE_UPDATE_TIME, SNAKE_MOVEMENT_TIME, 32);		break;	
		case GHOST:		updateGhostEnemy(&level->enemies[i], level->player, level->enemies, level->enemyCount, timePassed, GHOST_SPEED, MAX_GHOST_DIST);											break;
		case SKULL:  updateEnemy(&level->enemies[i], stepsToGoal, level->width, level->height, level->enemies, level->enemyCount, timePassed, SKULL_UPDATE_TIME, SKULL_MOVEMENT_TIME, 32); break;
		default:		break;
		}

		//Drop items upon death
		if(level->enemies[i].health <= 0)
		{
			//Drop coins / treasure
			for(int j = 0; j < getEnemyScoreVal(level->enemies[i].type); j++)
			{
				float dist = (float)rand() / (float)RAND_MAX * 0.4f,
					  angle = (float)rand() / (float)RAND_MAX * 3.141592653f;
				addItem(level, createItem(createSprite(level->enemies[i].animationX + dist * cosf(angle), level->enemies[i].animationY + dist * sinf(angle), 0.5f, 0.5f), COIN));
			}
		}
	}

	//Update the items in the level
	for(int i = 0; i < level->itemCount; i++)
	{
		if(level->items[i].hidden)
			continue;

		updateAnimationFrame(&level->items[i].hitbox, totalTime);
		//Move the player
		if(spriteDist(level->player.spr, level->items[i].hitbox) < 2.0f)
		{
			float diffX = level->player.spr.x - level->items[i].hitbox.x,
				  diffY = level->player.spr.y - level->items[i].hitbox.y;
			
			float moveX = 2.0f / sqrtf(diffX * diffX + diffY * diffY) * diffX,
				  moveY = 2.0f / sqrtf(diffX * diffX + diffY * diffY) * diffY;

			level->items[i].hitbox.x += moveX * timePassed;
			level->items[i].hitbox.y += moveY * timePassed;
		}
		//Player collected the item	
		if(colliding(level->player.spr, level->items[i].hitbox))
		{
			level->items[i].hidden = 1;
			switch(level->items[i].item)
			{
			case COIN:
				level->player.score++;
			default:
				break;
			}
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
