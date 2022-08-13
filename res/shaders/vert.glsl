#version 430

uniform vec2 uOffset = vec2(0.0, 0.0), uScale = vec2(1.0, 1.0); 
layout(location=0) in vec4 pos;
layout(location=1) in vec2 texCoords;
out vec2 tc;

void main()
{
	gl_Position = vec4(uOffset.xy, 0.0, 0.0) + vec4(pos.x * uScale.x, pos.y * uScale.y, 0.0, 1.0);
	tc = texCoords;
}
