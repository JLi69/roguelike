#ifndef INCLUDE_SPRITE
enum AnimationState
{
	IDLE_DOWN = 0,
	MOVING_DOWN = 1,
	IDLE_UP = 2,
	MOVING_UP = 3,
	IDLE_RIGHT = 4,
	MOVING_RIGHT = 5,
	IDLE_LEFT = 6,			
	MOVING_LEFT = 7
};

struct Sprite
{
	float x, y; //Position of the sprite
	float dX, dY; //Movement of the sprite	
	float width, height; //Dimension of the sprite
	
	enum AnimationState animation;
	int frame;
};

struct Player
{
	struct Sprite spr;	
	int health, maxHealth, //Amount of health the player has
		score; //Score of the player
};

struct Sprite createSprite(float x, float y, float width, float height);
int colliding(struct Sprite s1, struct Sprite s2);
void updateAnimationFrame(struct Sprite *spr, float totalTime);

#endif

#define INCLUDE_SPRITE
