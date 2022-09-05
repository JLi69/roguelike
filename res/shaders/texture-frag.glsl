#version 430

layout(binding=0) uniform sampler2D tex;

uniform float uDamageCooldown = 0.0f; //Amount of time since the player last took damage
uniform float uTexSize = 64.0; //Size of texture in pixels
uniform float uTexFrac = 1.0; //Fraction of the image is the texture
uniform float uHitCooldown = 0.0f; //Amount of time since an enemy took damage
uniform vec2 uTexOffset = vec2(0.0, 0.0); //Where the texture is in the image
out vec4 outputColor;
in vec2 tc;

void main()
{	
	float spriteSize = uTexFrac * uTexSize;	
	vec2 pixelTc = (0.5 + floor(((tc) * uTexFrac + uTexOffset) * uTexSize)) / uTexSize;	
	float texY = pixelTc.y - uTexOffset.y;

	if(texY * uTexSize >= spriteSize)
	{
		pixelTc = vec2(pixelTc.x, uTexOffset.y + uTexFrac - 0.5 / uTexSize);
	}
	
	outputColor = 
		texture(tex, pixelTc) + (vec4(1.0, 0.0, 0.0, 0.0) * (uDamageCooldown + uHitCooldown));	
}
