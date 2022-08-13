#version 430

layout(binding=0) uniform sampler2D tex;
out vec4 outputColor;
in vec2 tc;

void main()
{
	vec2 pixelTc = (0.5 + floor((tc) * 64.0)) / 64.0;	
	outputColor = texture(tex, pixelTc);	
}
