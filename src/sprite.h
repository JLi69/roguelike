#ifndef INCLUDE_SPRITE
enum AnimationState
{
	IDLE_DOWN,
	IDLE_UP,
	IDLE_LEFT,
	IDLE_RIGHT,
	MOVING_DOWN,
	MOVING_UP,
	MOVING_LEFT,
	MOVING_RIGHT
};

struct Sprite
{
	float x, y; //Position of the sprite
	float dX, dY; //Movement of the sprite	
	float width, height; //Dimension of the sprite
};
#endif

struct Sprite createSprite(float x, float y, float width, float height);
int colliding(struct Sprite s1, struct Sprite s2);

#define INCLUDE_SPRITE
