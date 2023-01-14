#ifndef INCLUDE_ENEMY
#include "sprite.h"

#define SLIME_MOVEMENT_TIME 0.25f
#define SLIME_UPDATE_TIME 1.0f
#define SNAKE_MOVEMENT_TIME 0.25f
#define SNAKE_UPDATE_TIME 0.75f
#define SKULL_MOVEMENT_TIME 0.25f
#define SKULL_UPDATE_TIME 0.6f
#define EYE_MOVEMENT_TIME 0.2f
#define EYE_UPDATE_TIME 0.7f

#define GHOST_SPEED 2.0f
#define MAX_GHOST_DIST 12.0f

//Number of types of enemies
#define ENEMY_TYPE_COUNT 6

enum EnemyType
{
	OPEN_CHEST_MONSTER,
	SLIME,
	SNAKE,
	GHOST,
	SKULL,
	EYEBALL
};

struct Enemy
{
	struct Sprite spr;
	//Not real positoin of enemy
	float animationX, animationY;
	float timer, damageCooldown;
	enum EnemyType type;	
	int health;
};

struct Enemy createEnemy(struct Sprite spr, enum EnemyType type);
//Update enemy functions

//void updateSlime(struct Enemy *slime, int *stepsToPlayer, int levelWidth, int levelHeight, struct Enemy *otherEnemies, int enemyCount, float timePassed);
//void updateSnake(struct Enemy *snake, int *stepsToPlayer, int levelWidth, int levelHeight, struct Enemy *otherEnemies, int enemyCount, float timePassed);

//Normal enemy update function
//Enemy attempts to follow player but cannot go through walls
//The enemy will fall into traps so the player can use that to
//their advantage
void updateEnemy(struct Enemy *enemy,
				 int *stepsToPlayer,
				 int levelWidth, int levelHeight,
				 struct Enemy *otherEnemies,
				 int enemyCount,
				 float timePassed, 
				 float updateTime, float movementTime,
				 int maxStepDist);

//Ghost enemy update function
//Enemy can go through walls and attempt to follow player
void updateGhostEnemy(struct Enemy *enemy,
					  struct Player player,
					  struct Enemy *otherEnemies,
					  int enemyCount, 
					  float timePassed,
					  float speed,
					  int maxDist);

int getEnemyScoreVal(enum EnemyType type);
int getEnemyDamageVal(enum EnemyType type);

#endif

#define INCLUDE_ENEMY
