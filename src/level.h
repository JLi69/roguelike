#ifndef INCLUDE_LEVEL
#include "sprite.h"
#define PASSAGE_LENGTH 26
#define MIN_ROOM_SIZE 8
#define MAX_ROOM_SIZE 16

enum Tile
{
	EMPTY,
	FLOOR,
	WALL
};

//Level
//Contains the tiles, enemies, traps, and powerups 
typedef struct
{
	//Dimensions
	int width, height;
	//Tiles - these represent the map	
	enum Tile* tiles;
	//Random values	
	short* randVals;

	//Player sprite
	struct Sprite player;	
} Level;

//Pass in the seed and level number
//As level number gets bigger, the level gets larger
//and more enemies and traps get generated
Level* genLevel(int seed, int levelNum);
//Deallocate memory that represents a level
void destroyLevel(Level *level);

#endif

#define INCLUDE_LEVEL
