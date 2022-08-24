#version 430

#define TILE_SIZE 48.0
#define WIDTH 49
#define HEIGHT 33

uniform vec2 uScale = vec2(1.0, 1.0),
			 uScreenDimensions = vec2(960.0f, 540.0f); 
layout(location=0) in vec4 pos;
layout(location=1) in vec2 texCoords;
out vec2 tc;

void main()
{
	//Background will always be a 48 x 32 grid of the same squares	
	vec2 offset = vec2(((gl_InstanceID % WIDTH) * TILE_SIZE - WIDTH / 2 * TILE_SIZE),
					   ((gl_InstanceID / WIDTH) * TILE_SIZE - HEIGHT / 2 * TILE_SIZE)) * 2.0;

	gl_Position = vec4(offset.x / uScreenDimensions.x, offset.y / uScreenDimensions.y, 0.0, 0.0) +
				  vec4(pos.x * uScale.x / uScreenDimensions.x, pos.y * uScale.y / uScreenDimensions.y, 0.0, 1.0);
	tc = texCoords;	
}
