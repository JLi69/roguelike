#include "level.h"
#include <stdlib.h>
#include "structs.h"
#include <stdio.h>
#include <math.h>

void addEnemy(Level *level, struct Enemy enemy)
{
	//Enough room for the enemy
	if(level->enemyCount < level->maxEnemyCount)
		level->enemies[level->enemyCount++] = enemy;
	else if(level->enemyCount >= level->maxEnemyCount)
	{
		//Temporary array to store the enemies	
		struct Enemy* temp = (struct Enemy*)malloc(level->enemyCount * sizeof(struct Enemy));
		for(int i = 0; i < level->enemyCount; i++)
			temp[i] = level->enemies[i];
		
		//Expand the array
		free(level->enemies);
		level->maxEnemyCount *= 2;	
		level->enemies = (struct Enemy*)malloc(level->maxEnemyCount * sizeof(struct Enemy));

		//Copy the enemies back into the array
		for(int i = 0; i < level->enemyCount; i++)
			level->enemies[i] = temp[i];
		level->enemies[level->enemyCount++] = enemy;

		free(temp);
	}
}

void addItem(Level *level, struct Item item)
{
	//Enough room for the item
	if(level->itemCount < level->maxItemCount)
		level->items[level->itemCount++] = item;
	else if(level->itemCount >= level->maxItemCount)
	{
		//Temporary array to store the items	
		struct Item* temp = (struct Item*)malloc(level->itemCount * sizeof(struct Item));
		for(int i = 0; i < level->itemCount; i++)
			temp[i] = level->items[i];
		
		//Expand the array
		free(level->items);
		level->maxItemCount *= 2;	
		level->items = (struct Item*)malloc(level->maxItemCount * sizeof(struct Item));

		//Copy the items back into the array
		for(int i = 0; i < level->itemCount; i++)
			level->items[i] = temp[i];
		level->items[level->itemCount++] = item;

		free(temp);
	}
}

Level* genLevel(unsigned int seed, int levelNum)
{
	//Allocate memory for a level	
	Level* level = (Level*)malloc(sizeof(Level));

	//Set the seed of the level
	srand(seed);

	//Generate the size of the level
	level->width = (96 + 16 * levelNum) + rand() % (64 + 8 * levelNum); 
	level->height = (96 + 16 * levelNum) + rand() % (64 + 8 * levelNum);
	level->tiles = (enum Tile*)malloc(level->width * level->height * sizeof(enum Tile));
	level->randVals = (short*)malloc(level->width * level->height * sizeof(int));

	//Allocate memory for enemies
	level->enemies = (struct Enemy*)malloc(sizeof(struct Enemy));
	level->enemyCount = 0;
	level->maxEnemyCount = 1;

	//Allocate memory for items
	level->items = (struct Item*)malloc(sizeof(struct Item));
	level->itemCount = 0;
	level->maxItemCount = 1;

	//Array to keep track of how many times a cell was visited
	short* timesVisited = (short*)malloc(level->width * level->height * sizeof(short));
	for(int i = 0; i < level->width * level->height; i++)
		timesVisited[i] = 0;

	//Fill in the level tiles
	for(int i = 0; i < level->width * level->height; i++)
		level->tiles[i] = WALL;

	//Generate hallways		
	int startX = rand() % (level->width - MAX_ROOM_SIZE * 2 - 3) + MAX_ROOM_SIZE + 2,
		startY = rand() % (level->height - MAX_ROOM_SIZE * 2 - 3) + MAX_ROOM_SIZE + 2;	
	Stack* cells = stack_create();
	Stack* deadEnds = stack_create();
	Stack* potentialRooms = stack_create();

	//Push the starting position onto the stack
	stack_push(cells, startX);
	stack_push(cells, startY);

	const int changeX[] = { -PASSAGE_LENGTH, PASSAGE_LENGTH,				 0,				 0 },
			  changeY[] = {				  0,			  0,   -PASSAGE_LENGTH, PASSAGE_LENGTH };

	while(!stack_empty(cells))
	{
		//Get the coordinates at the top of the stack	
		int y = stack_pop(cells),
			x = stack_pop(cells);
		//Put the cell back
		stack_push(cells, x);
		stack_push(cells, y);

		//Generate a direction
		int direction = rand() % 4,
			found = 0;
		//Try to find a valid cell to go to
		for(int i = 0; i < 4 && !found; i++)
		{
			int potentialDirection = (direction + i) % 4,
				diffX = changeX[potentialDirection], diffY = changeY[potentialDirection];
			
			//Check to make sure the cell is within bounds
			if(x + diffX < MAX_ROOM_SIZE / 2 + 2 || 
			   y + diffY < MAX_ROOM_SIZE / 2 + 2 || 
			   x + diffX >= level->width - MAX_ROOM_SIZE / 2 - 2 ||
			   y + diffY >= level->height - MAX_ROOM_SIZE / 2 - 2)
				continue;	
			//Make sure that it hasn't been visited already (can only visit a cell up to 2 times)
			//if(level->tiles[(x + diffX) + (y + diffY) * level->width] != FLOOR)
			if(timesVisited[(x + diffX) + (y + diffY) * level->width] < 2)
			{
				timesVisited[(x + diffX) + (y + diffY) * level->width]++;	
				found = 1;
				direction = potentialDirection;
			}	
		} 

		//Found a cell, "carve" a hallway to there
		if(found)
		{
			int minX = changeX[direction] < 0 ? x + changeX[direction] : x,
				minY = changeY[direction] < 0 ? y + changeY[direction] : y,
				maxX = changeX[direction] > 0 ? x + changeX[direction] : x,
				maxY = changeY[direction] > 0 ? y + changeY[direction] : y;

			for(int i = minX - 1; i <= maxX + 1; i++)
				for(int j = minY - 1; j <= maxY + 1; j++)
					level->tiles[i + j * level->width] = FLOOR;

			//Push the new position onto the stack
			stack_push(cells, x + changeX[direction]);
			stack_push(cells, y + changeY[direction]);
			
		}
		//Didn't find a cell, pop off the cell at the top of the stack
		else if(!found)
		{
			stack_pop(cells);
			stack_pop(cells);		

			//Check if the cell is a dead end
			const int indexOffset[] = { -2, 2, -2 * level->width, 2 * level->width };
			int count = 0;
			for(int i = 0; i < 4; i++)
				if(level->tiles[x + y * level->width + indexOffset[i]] == FLOOR)
					count++;	
			//Dead end
			if(count <= 1)
			{
				//Add it to the dead end stack	
				stack_push(deadEnds, x);
				stack_push(deadEnds, y);
			}	
			//Not a dead end
			else if(count > 1)
			{
				//Add it to the potential room stack	
				stack_push(potentialRooms, x);
				stack_push(potentialRooms, y);
			}
		}
	}

	stack_destroy(cells);

	//Generate Rooms
	while(!stack_empty(deadEnds))
	{
		int y = stack_pop(deadEnds),
			x = stack_pop(deadEnds);

		//Generate a random room size
		int width = rand() % (MAX_ROOM_SIZE / 2 + 1 - MIN_ROOM_SIZE / 2) + MIN_ROOM_SIZE / 2,
			height = rand() % (MAX_ROOM_SIZE / 2 + 1 - MIN_ROOM_SIZE / 2) + MIN_ROOM_SIZE / 2;
		//"Carve" out the room
		for(int i = y - height; i <= y + height; i++)
			for(int j = x - width; j <= x + width; j++)
				level->tiles[i * level->width + j] = FLOOR;
	}
	
	while(!stack_empty(potentialRooms))
	{
		int y = stack_pop(potentialRooms),
			x = stack_pop(potentialRooms);

		//Generate a random room half the time
		if(rand() % 2 == 0 || (x == startX && y == startY) || stack_empty(potentialRooms))
		{	
			//Generate a random size for the room	
			int width = rand() % (MAX_ROOM_SIZE / 2 + 1 - MIN_ROOM_SIZE / 2) + MIN_ROOM_SIZE / 2,
				height = rand() % (MAX_ROOM_SIZE / 2 + 1 - MIN_ROOM_SIZE / 2) + MIN_ROOM_SIZE / 2;
			//"Carve" out the room
			for(int i = y - height; i <= y + height; i++)
				for(int j = x - width; j <= x + width; j++)
					level->tiles[i * level->width + j] = FLOOR;

			//Generate the exit if it is the last room
			if(stack_empty(potentialRooms))
				level->tiles[rand() % (width * 2) + x - width + (rand() % (2 * height) + y - height) * level->width] = EXIT;
		}	
	}

	//Get rid of walls that do not border a room/hallway
	for(int i = 0; i < level->height; i++)
	{	
		for(int j = 0; j < level->width; j++)
		{		
			const int indexOffset[] = 
				{ -1, 1, -level->width, level->width,
				  -level->width - 1, level->width - 1, -level->width + 1, level->width + 1 };
			//How many floors border the tile	
			int count = 0;
			for(int ind = 0; ind < 8; ind++)
			{
				//Check if it is within bounds	
				if(j + i * level->width + indexOffset[ind] < 0 ||
				   j + i * level->width + indexOffset[ind] >= level->height * level->width)	
					continue;
				//Increment the count by 1 if the neighboring tile is a floor	
				count += level->tiles[j + i * level->width + indexOffset[ind]] == FLOOR;
			}

			//Remove the tile if it doesn't meet the criteria	
			if(i == 0 || i == level->height - 1 ||
			   (level->tiles[j + i * level->width] == WALL && count == 0))
				level->tiles[j + i * level->width] = EMPTY;
		}
	}

	//Randomly place items around the map
	for(int i = 0; i < level->width * level->height; i++)
	{
		if(rand() % 256 == 0 && level->tiles[i] == FLOOR)
		{
			if(levelNum >= 8)
			{
				level->tiles[i] = CHEST_MONSTER;
				continue;
			}

			if(rand() % (16 - 2 * levelNum) == 0)	level->tiles[i] = CHEST_MONSTER;
			else level->tiles[i] = CHEST;
		}
		else if(rand() % 16 == 0 && i / level->width % 3 == 0 && (i % level->width) % 3 == 0 && level->tiles[i] == FLOOR &&
				(i / level->width != startY || i % level->width != startX)) //Don't spawn spike trap where player starts
		{
			level->tiles[i] = SPIKE_TRAP_INACTIVE;
		}
	}

	for(int i = 0; i < level->width * level->height; i++)
		level->randVals[i] = rand() % 64;	

	//Generate the enemies
	for(int i = 0; i < level->width; i++)
		for(int j = 0; j < level->height; j++)
			if(level->tiles[i + j * level->width] == FLOOR && rand() % 72 == 0 &&
				(labs(startX - i) > 6 || labs(startY - j) > 6)) //Don't spawn too close the player
				addEnemy(level, createEnemy(createSprite(i, j, 1.0f, 1.0f), SLIME));	

	//Deallocate the stacks
	stack_destroy(potentialRooms);
	stack_destroy(deadEnds);

	free(timesVisited);

#if 0
	//For debug purposes, print out the level	
	for(int i = 0; i < level->width * level->height; i++)
	{
		const char tileCh[] = { ' ', '.', '#', 'E' };
		putchar(tileCh[level->tiles[i]]);
		if(i % level->width == level->width - 1)
			putchar('\n');
	}
#endif

	//Player sprite
	level->player.spr = createSprite((float)startX, (float)startY, 1.0f, 1.0f);
	level->player.health = 10;
	level->player.maxHealth = 10;
	level->player.score = 0;	

	level->state = PLAYING;

	return level;
}

void destroyLevel(Level *level)
{
	free(level->tiles);
	free(level->enemies);
	free(level->items);
	free(level->randVals);
	free(level);
}

void bfs(int *stepsToGoal, Level level)
{
	int* tileQueue = (int*)malloc(sizeof(int) * level.width * level.height * 3);
	int* added = (int*)malloc(sizeof(int) * level.width * level.height);
	int first = 0, last = 0;

	for(int i = 0; i < level.width * level.height; i++)
		added[i] = 0;
	for(int i = 0; i < level.width * level.height; i++)
		stepsToGoal[i] = -1;

	//Add the initial tile to the tile queue
	tileQueue[last++] = (int)floorf(level.player.spr.x);
	tileQueue[last++] = (int)floorf(level.player.spr.y);
	tileQueue[last++] = 0;
	added[32 + 32 * 65] = 1;

	//Keep doing the bfs until queue is empty
	static const int diffX[] = { 1,  -1, 0,  0 };
	static const int diffY[] = { 0,   0, 1, -1 };
	
	while(last > first)
	{
		int tileX = tileQueue[first++],
			tileY = tileQueue[first++],
			steps = tileQueue[first++];

		int tileIndex = tileX + tileY * level.width;
		stepsToGoal[tileIndex] = steps;

		for(int i = 0; i < 4; i++)
		{
			int newTileIndex = (tileX + diffX[i]) + (tileY + diffY[i]) * level.width;
			if(newTileIndex < 0 || newTileIndex >= level.width * level.height)
				continue;
			if(level.tiles[newTileIndex] != FLOOR && 
			   level.tiles[newTileIndex] != SPIKE_TRAP_INACTIVE &&
			   level.tiles[newTileIndex] != SPIKE_TRAP_ACTIVE)
				continue;
			if(added[newTileIndex])
				continue;
			if(stepsToGoal[newTileIndex] != -1)
				continue;
			if(steps + 1 > 128)
				continue;
			
			tileQueue[last++] = tileX + diffX[i];
			tileQueue[last++] = tileY + diffY[i];
			tileQueue[last++] = steps + 1;
			added[newTileIndex] = 1;
		}
	}

	free(tileQueue);
	free(added);
}
