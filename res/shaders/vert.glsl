#version 430

uniform vec2 uOffset = vec2(0.0, 0.0), uScale = vec2(1.0, 1.0),
			 uScreenDimensions = vec2(960.0f, 540.0f); 
layout(location=0) in vec4 pos;
layout(location=1) in vec2 texCoords;
out vec2 tc;

void main()
{
	gl_Position = 
		vec4(uOffset.x / uScreenDimensions.x * 2.0, uOffset.y / uScreenDimensions.y * 2.0, 0.0, 0.0) +
		vec4(pos.x * uScale.x / uScreenDimensions.x, pos.y * uScale.y / uScreenDimensions.y, 0.0, 1.0);
	tc = texCoords;
}
