#include "sprite.h"
#include <math.h>

#define ANIMATION_TIME 0.5f

struct Sprite createSprite(float x, float y, float width, float height)
{
	struct Sprite spr;
	spr.x = x;
	spr.y = y;

	spr.dX = 0.0f;
	spr.dY = 0.0f;

	spr.width = width;
	spr.height = height;

	spr.animation = IDLE_DOWN;
	spr.frame = 0;

	return spr;
}

int colliding(struct Sprite s1, struct Sprite s2)
{
	return s1.x - s1.width / 2.0f < s2.x + s2.width / 2.0f &&
		   s1.x + s1.width / 2.0f > s2.x - s2.width / 2.0f &&
		   s1.y - s1.height / 2.0f < s2.y + s2.height / 2.0f &&
		   s1.y + s1.height / 2.0f > s2.y - s2.height / 2.0f;
}

void updateAnimationFrame(struct Sprite *spr, float totalTime)
{	
	if((int)floorf(totalTime / ANIMATION_TIME) % 2 == 0)
		spr->frame = 0;
	else if((int)floorf(totalTime / ANIMATION_TIME) % 2 == 1)
		spr->frame = 1;
}
