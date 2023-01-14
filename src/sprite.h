#ifndef INCLUDE_SPRITE
#define ATTACK_COOLDOWN_LENGTH 0.15f

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

enum Items
{
	COIN,
	WOOD_SWORD
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
	float attackCooldown;
};

struct Item
{
	struct Sprite hitbox;
	int value, hidden;
	enum Items item;
};

struct Sprite createSprite(float x, float y, float width, float height);
struct Item createItem(struct Sprite hitbox, enum Items item);
int colliding(struct Sprite s1, struct Sprite s2);
float spriteDist(struct Sprite s1, struct Sprite s2);
void updateAnimationFrame(struct Sprite *spr, float totalTime);
void updateAnimationFrameCustom(struct Sprite *spr, float totalTime, int frameCount, float animTime);
void updateAnimationFrameCustomLoop(struct Sprite *spr, float totalTime, int frameCount, float animTime);

#endif

#define INCLUDE_SPRITE
