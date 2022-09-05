#include "text.h"
#include <math.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <stdio.h>
#include "roguelike.h"

int getDigits(int num)
{
	if(num == 0) return 1;
	return (num > 0) ? (int)log10(num) + 1 : (int)log10(-num) + 1;
}

void drawNumber(float x, float y, float scale, int value)
{
	char* digits = (char*)malloc(sizeof(char) * getDigits(value));
	sprintf(digits, "%d", value);
	
	float digitX = x, digitY = y;
	glUniform2f(getUniform(DEFAULT_UNIFORM_SCALE), scale, scale);	
	for(int i = 0; i < getDigits(value); i++)
	{			
		glUniform2f(getUniform(DEFAULT_UNIFORM_OFFSET), digitX, digitY);
		glUniform2f(getUniform(DEFAULT_UNIFORM_TEX_OFFSET),
					1.0f / 16.0f * (digits[i] - '0'),
					14.0f / 16.0f);	
		digitX += scale;
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	free(digits);
}
