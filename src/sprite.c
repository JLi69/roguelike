#include "sprite.h"
#include <math.h>

#define ANIMATION_TIME 0.5f

//Coin value of various items
static const int itemValues[] =
{
	1, //Coin
	2, //Wooden sword
};

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

struct Item createItem(struct Sprite hitbox, enum Items itemType)
{
	struct Item item;

	item.hitbox = hitbox;
	item.item = itemType;
	item.value = itemValues[(int)itemType];
	item.hidden = 0;

	return item;
}

int colliding(struct Sprite s1, struct Sprite s2)
{
	return s1.x - s1.width / 2.0f < s2.x + s2.width / 2.0f &&
		   s1.x + s1.width / 2.0f > s2.x - s2.width / 2.0f &&
		   s1.y - s1.height / 2.0f < s2.y + s2.height / 2.0f &&
		   s1.y + s1.height / 2.0f > s2.y - s2.height / 2.0f;
}

float spriteDist(struct Sprite s1, struct Sprite s2)
{
	return sqrtf((s1.x - s2.x) * (s1.x - s2.x) + (s1.y - s2.y) * (s1.y - s2.y));
}

void updateAnimationFrame(struct Sprite *spr, float totalTime)
{	
	spr->frame = (int)floorf(totalTime / ANIMATION_TIME) % 2; 
}

void updateAnimationFrameCustom(struct Sprite *spr, float totalTime, int frameCount, float animTime)
{	
	spr->frame = (int)floorf(totalTime / animTime) % frameCount; 
}

void updateAnimationFrameCustomLoop(struct Sprite *spr, float totalTime, int frameCount, float animTime)
{
	int frame = (int)floorf(totalTime / animTime);
	frame %= (2 * frameCount);

	if(frame < frameCount) spr->frame = frame;
	else if(frame >= frameCount) spr->frame = frameCount - 1 - frame % frameCount;
}
