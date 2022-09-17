#include "enemy.h"
#include <math.h>

static const int enemyHealthTable[] = 
{
	4, //Chest monster
	2, //Slime
};

static const int enemyScoreTable[] =
{
	12, //Chest monster
	1, //Slime
};

static const int enemyHitTable[] =
{
	2, //Chest monster
	1 //Slime
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

void updateSlime(struct Enemy *slime, int *stepsToPlayer, int levelWidth, int levelHeight, struct Enemy *otherEnemies, int enemyCount, float timePassed)
{
	int slimeX = (int)floorf(slime->spr.x),
		slimeY = (int)floorf(slime->spr.y);
	int steps = stepsToPlayer[slimeX + slimeY * levelWidth];		

	slime->animationX = 
		(slime->timer < SLIME_MOVEMENT_TIME) ?
		slime->spr.x - slime->spr.dX * (SLIME_MOVEMENT_TIME - slime->timer) / SLIME_MOVEMENT_TIME :
		slime->spr.x;
	slime->animationY = 
		(slime->timer < SLIME_MOVEMENT_TIME) ?		
		slime->spr.y - slime->spr.dY * (SLIME_MOVEMENT_TIME - slime->timer) / SLIME_MOVEMENT_TIME :
		slime->spr.y;

	if(steps > 16)
		return;

	slime->timer += timePassed;
	if(slime->timer < SLIME_UPDATE_TIME)
		return;	
	else if(slime->timer >= SLIME_UPDATE_TIME)
		slime->timer = 0.0f;

	slime->spr.dX = 0.0f;
	slime->spr.dY = 0.0f;
	for(int i = 0; i < 4; i++)
	{
		int index = slimeX + diffX[i] + (slimeY + diffY[i]) * levelWidth;		
		//Out of bounds	
		if(index < 0 || index >= levelWidth * levelHeight) 
			continue;
	
		//Test to see if it is occupied
		if(stepsToPlayer[index] == -1)
			continue;	

		
		//Make sure that the spot it isn't occupied by other enemies
		int occupied = 0;	
		for(int j = 0; j < enemyCount && !occupied; j++)	
			if(otherEnemies[j].spr.x == slimeX + diffX[i] &&
			   otherEnemies[j].spr.y == slimeY + diffY[i] &&
			   otherEnemies[j].health > 0)
				occupied = 1;

		if(stepsToPlayer[index] == steps - 1 && !occupied)
		{
			slime->spr.dX = diffX[i];
			slime->spr.dY = diffY[i];
		}
	}

	//Move the slime	
	slime->spr.x += slime->spr.dX;
	slime->spr.y += slime->spr.dY;	
}

int getEnemyScoreVal(enum EnemyType type)
{
	return enemyScoreTable[(int)type];
}

int getEnemyDamageVal(enum EnemyType type)
{
	return enemyHitTable[(int)type];
}
