#ifndef INCLUDE_LEVEL
#include "sprite.h"
#include "enemy.h" 
#define PASSAGE_LENGTH 26
#define MIN_ROOM_SIZE 8
#define MAX_ROOM_SIZE 16

enum Tile
{
	EMPTY,
	FLOOR,
	WALL,
	EXIT,
	CHEST,
	CHEST_MONSTER,
	OPEN_CHEST,
	SPIKE_TRAP_INACTIVE,
	SPIKE_TRAP_ACTIVE
};

enum GameState
{
	PLAYING,
	DEAD,
	NEXT_LEVEL
};

//Level
//Contains the tiles, enemies, traps, and powerups 
typedef struct
{
	//Dimensions
	int width, height;
	//Tiles - these represent the map	
	enum Tile* tiles;
	enum GameState state;
	//Array of enemies
	struct Enemy* enemies;	
	int enemyCount, maxEnemyCount;	
	struct Item* items;
	int itemCount, maxItemCount;
	//Random values	
	short* randVals;

	//Player sprite
	struct Player player;	
} Level;

//Add an enemy to a level
void addEnemy(Level *level, struct Enemy enemy);
//Add items to the level
void addItem(Level *level, struct Item item);
//Pass in the seed and level number
//As level number gets bigger, the level gets larger
//and more enemies and traps get generated
Level* genLevel(unsigned int seed, int levelNum);
//Deallocate memory that represents a level
void destroyLevel(Level *level);
//Output an array of values that represent the number of steps it will take
//to get to the player from a point using bfs
void bfs(int *stepsToGoal, Level level);

#endif

#define INCLUDE_LEVEL
