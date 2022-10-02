#include "enemy.h"
#include <math.h>

#include <stdio.h>

static const int enemyHealthTable[] = 
{
	4, //Chest monster
	2, //Slime
	4, //Snake
	8, //Ghost
	6, //Skull
};

static const int enemyScoreTable[] =
{
	12, //Chest monster
	1, //Slime
	2, //Snake
	5, //Ghost
	3, //Skull
};

static const int enemyHitTable[] =
{
	2, //Chest monster
	1, //Slime
	2, //Snake
	1, //Ghost
	3, //Skull
};

static const int diffX[] = {  -1,  0,   1,   0 };
static const int diffY[] = {   0,  1,   0,  -1 };

struct Enemy createEnemy(struct Sprite spr, enum EnemyType type)
{
	struct Enemy temp;

	temp.timer = 0.0f;
	temp.damageCooldown = 0.0f;

	temp.spr = spr;
	temp.type = type;
	temp.animationX = spr.x;
	temp.animationY = spr.y;
	//Set the health of the enemy
	temp.health = enemyHealthTable[(int)type];	

	return temp;
}

void updateEnemy(struct Enemy *enemy,
				 int *stepsToPlayer,
				 int levelWidth, int levelHeight,
				 struct Enemy *otherEnemies,
				 int enemyCount,
				 float timePassed, 
				 float updateTime, float movementTime,
				 int maxStepDist)
{
	int enemyX = (int)floorf(enemy->spr.x),
		enemyY = (int)floorf(enemy->spr.y);
	int steps = stepsToPlayer[enemyX + enemyY * levelWidth];		

	enemy->animationX = 
		(enemy->timer < movementTime) ?
		enemy->spr.x - enemy->spr.dX * (movementTime - enemy->timer) / movementTime :
		enemy->spr.x;
	enemy->animationY = 
		(enemy->timer < movementTime) ?		
		enemy->spr.y - enemy->spr.dY * (movementTime - enemy->timer) / movementTime :
		enemy->spr.y;

	if(steps > maxStepDist)
		return;

	enemy->timer += timePassed;
	if(enemy->timer < updateTime)
		return;	
	else if(enemy->timer >= updateTime)
		enemy->timer = 0.0f;

	enemy->spr.dX = 0.0f;
	enemy->spr.dY = 0.0f;
	for(int i = 0; i < 4; i++)
	{
		int index = enemyX + diffX[i] + (enemyY + diffY[i]) * levelWidth;		
		//Out of bounds	
		if(index < 0 || index >= levelWidth * levelHeight) 
			continue;
	
		//Test to see if it is occupied
		if(stepsToPlayer[index] == -1)
			continue;	
		
		//Make sure that the spot it isn't occupied by other enemies
		int occupied = 0;	
		for(int j = 0; j < enemyCount && !occupied; j++)	
			if(otherEnemies[j].spr.x == enemyX + diffX[i] &&
			   otherEnemies[j].spr.y == enemyY + diffY[i] &&
			   otherEnemies[j].health > 0)
				occupied = 1;

		if(stepsToPlayer[index] == steps - 1 && !occupied)
		{
			enemy->spr.dX = diffX[i];
			enemy->spr.dY = diffY[i];
		}
	}

	//Move the enemy	
	enemy->spr.x += enemy->spr.dX;
	enemy->spr.y += enemy->spr.dY;
}

void updateGhostEnemy(struct Enemy *enemy,
					  struct Player player,
					  struct Enemy *otherEnemies,
					  int enemyCount, 
					  float timePassed,
					  float speed,
					  int maxDist)
{
	float distSq = (enemy->spr.x - player.spr.x) * (enemy->spr.x - player.spr.x) +
				   (enemy->spr.y - player.spr.y) * (enemy->spr.y - player.spr.y);

	//Enemy is too far away
	if(distSq > maxDist * maxDist)
		return;

	//Move the enemy
	float diffX = (enemy->spr.x - player.spr.x),
		  diffY = (enemy->spr.y - player.spr.y);

	diffX *= speed / sqrtf(distSq);
	diffY *= speed / sqrtf(distSq);

	//Make sure the enemy doesn't get too close to other enemies
	for(int i = 0; i < enemyCount; i++)
	{
		float enemyDistSq = 
			(enemy->spr.x - diffX * timePassed - otherEnemies[i].spr.x) *
			(enemy->spr.x - diffX * timePassed - otherEnemies[i].spr.x) +  
			(enemy->spr.y - diffY * timePassed - otherEnemies[i].spr.y) * 
			(enemy->spr.y - diffY * timePassed - otherEnemies[i].spr.y);	
		//Probably the same enemy so ignore		
		if(enemyDistSq <= 0.02f) continue;
		//Ignore dead enemies
		if(otherEnemies[i].health <= 0) continue;
		if(otherEnemies[i].type != enemy->type) continue;

		if(enemyDistSq < 1.0f)
			return;
	}

	enemy->spr.x -= diffX * timePassed;
	enemy->spr.y -= diffY * timePassed;
	
	enemy->animationX = enemy->spr.x;
	enemy->animationY = enemy->spr.y;
}

int getEnemyScoreVal(enum EnemyType type)
{
	return enemyScoreTable[(int)type];
}

int getEnemyDamageVal(enum EnemyType type)
{
	return enemyHitTable[(int)type];
}
