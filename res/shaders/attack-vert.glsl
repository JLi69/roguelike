#version 430

layout(location = 0) in vec4 pos;
out vec2 fragPos;
void main()
{
	gl_Position = pos;
	fragPos = pos.xy;
}
