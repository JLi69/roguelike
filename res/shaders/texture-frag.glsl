#version 430

uniform float uTexSize = 64.0; //Size of texture in pixels
uniform float uTexFrac = 1.0; //Fraction of the image is the texture
uniform vec2 uTexOffset = vec2(0.0, 0.0); //Where the texture is in the image
layout(binding=0) uniform sampler2D tex;
out vec4 outputColor;
in vec2 tc;

void main()
{
	vec2 pixelTc = (0.5 + floor((tc * uTexFrac + uTexOffset) * uTexSize)) / uTexSize;	
	outputColor = texture(tex, pixelTc);	
}
