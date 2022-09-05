#ifndef INCLUDE_ENEMY
#include "sprite.h"

#define SLIME_MOVEMENT_TIME 0.25f
#define SLIME_UPDATE_TIME 1.0f

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
	float timer, damageCooldown;
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
