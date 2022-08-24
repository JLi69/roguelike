#ifndef INCLUDE_ENEMY
#include "sprite.h"

#define SLIME_MOVEMENT_TIME 0.5f
#define SLIME_UPDATE_TIME 2.0f

enum EnemyType
{
	OPEN_CHEST_MONSTER,
	SLIME
};

struct Enemy
{
	struct Sprite spr;
	//Not real positoin of enemy
	float animationX, animationY;
	float timer;
	enum EnemyType type;	
	int health;	
};

struct Enemy createEnemy(struct Sprite spr, enum EnemyType type);
//Update enemy functions
void updateSlime(struct Enemy *slime, int *stepsToPlayer, int levelWidth, int levelHeight, struct Enemy *otherEnemies, int enemyCount, float timePassed);

int getEnemyScoreVal(enum EnemyType type);
int getEnemyDamageVal(enum EnemyType type);

#endif

#define INCLUDE_ENEMY
