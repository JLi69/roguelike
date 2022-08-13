#include "level.h"
#include <stdlib.h>
#include "structs.h"
#include <stdio.h>

Level* genLevel(int seed, int levelNum)
{
	//Allocate memory for a level	
	Level* level = (Level*)malloc(sizeof(Level));

	srand(seed);

	//Generate the size of the level
	level->width = (96 + 16 * levelNum) + rand() % (64 + 8 * levelNum); 
	level->height = (96 + 16 * levelNum) + rand() % (64 + 8 * levelNum);
	level->tiles = (enum Tile*)malloc(level->width * level->height * sizeof(enum Tile));

	//Fill in the level tiles
	for(int i = 0; i < level->width * level->height; i++)
		level->tiles[i] = WALL;

	//Generate hallways		
	int startX = rand() % (level->width - MAX_ROOM_SIZE - 2) + MAX_ROOM_SIZE / 2 + 1,
		startY = rand() % (level->height - MAX_ROOM_SIZE - 2) + MAX_ROOM_SIZE / 2 + 1;	
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
			if(x + diffX < MAX_ROOM_SIZE / 2 + 1 || 
			   y + diffY < MAX_ROOM_SIZE / 2 + 1 || 
			   x + diffX >= level->width - MAX_ROOM_SIZE / 2 - 1 ||
			   y + diffY >= level->height - MAX_ROOM_SIZE / 2 - 1)
				continue;	
			//Make sure that it hasn't been visited already
			if(level->tiles[(x + diffX) + (y + diffY) * level->width] != FLOOR)
			{
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

			for(int i = minX; i <= maxX; i++)
				for(int j = minY; j <= maxY; j++)
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
			const int indexOffset[] = { -1, 1, -level->width, level->width };
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
		if(rand() % 2 == 0 || (x == startX && y == startY))
		{
			//Generate a random size for the room	
			int width = rand() % (MAX_ROOM_SIZE / 2 + 1 - MIN_ROOM_SIZE / 2) + MIN_ROOM_SIZE / 2,
				height = rand() % (MAX_ROOM_SIZE / 2 + 1 - MIN_ROOM_SIZE / 2) + MIN_ROOM_SIZE / 2;
			//"Carve" out the room
			for(int i = y - height; i <= y + height; i++)
				for(int j = x - width; j <= x + width; j++)
					level->tiles[i * level->width + j] = FLOOR;
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

	//Deallocate the stacks
	stack_destroy(potentialRooms);
	stack_destroy(deadEnds);

	//For debug purposes, print out the level
	//for(int i = 0; i < level->width * level->height; i++)
	//{
	//	const char tileCh[] = { ' ', '.', '#' };
	//	putchar(tileCh[level->tiles[i]]);
	//	if(i % level->width == level->width - 1)
	//		putchar('\n');
	//}		
	
	return level;
}

void destroyLevel(Level *level)
{
	free(level->tiles);	
	free(level);
}
