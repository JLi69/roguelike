#version 430

#define MAX_ALPHA 0.4

#define ATTACK_RADIUS 120.0
#define WIDTH 60.0
//In seconds
#define ANIMATION_LENGTH 0.15
uniform vec2 uScreenDimensions = vec2(960.0, 540.0);
uniform float uAnimationTime = -0.1;

#define UP    0
#define DOWN  1
#define RIGHT 2
#define LEFT  3
uniform int uDirection = LEFT;

const vec2 centers[] = 
{
	vec2(0.0, WIDTH),
	vec2(0.0, -WIDTH),
	vec2(-WIDTH, 0.0),
	vec2(WIDTH, 0.0)
};

out vec4 color;
in vec2 fragPos;

void main()
{
	if(uAnimationTime <= 0.0f)
	{
		color = vec4(0.0, 0.0, 0.0, 0.0);	
		return;
	}

	float x = fragPos.x * uScreenDimensions.x,
		  y = fragPos.y * uScreenDimensions.y;

	float angle = atan(y / x);
	
	if(x > 0.0 && y < 0.0)
		angle += 3.14159 * 2.0;
	else if(x < 0.0 && y < 0.0)
		angle += 3.14159;
	else if(x < 0.0 && y > 0.0)
		angle += 3.14159;

	switch(uDirection)
	{
	case UP: angle -= 3.14159; break;
	case RIGHT: 
		if(angle > 3.14159 * 3.0 / 2.0)
			angle -= 3.14159 * 3.0 / 2.0;
		else if(angle < 3.14159 / 2.0)
			angle += 3.14159 / 2.0;
		break;
	case DOWN: break;
	case LEFT: angle -= 3.14159 / 2.0; break;
	default: break;
	}

	bool less = angle / 3.14159 < uAnimationTime / ANIMATION_LENGTH;

	float alpha = (angle / 3.14159) / (uAnimationTime / ANIMATION_LENGTH) * MAX_ALPHA;

	if(x * x + y * y < ATTACK_RADIUS * ATTACK_RADIUS &&
	   (x - centers[uDirection].x) * (x - centers[uDirection].x) +
	   (y - centers[uDirection].y) * (y - centers[uDirection].y) > ATTACK_RADIUS * ATTACK_RADIUS &&
	   less)
		color = vec4(1.0, 1.0, 1.0, alpha);
	else color = vec4(0.0, 0.0, 0.0, 0.0);
}
