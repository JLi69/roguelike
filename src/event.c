#include "event.h"
#include <glad/glad.h>
#include "gl-func.h"
#include "roguelike.h"

static int keys[4] = { -1, -1, -1, -1 };
static int winWidth = 960, winHeight = 540;

void onKeyPress(GLFWwindow *win, int key, int scancode, int action, int mods)
{
	//Pressed key	
	if(action == GLFW_PRESS)
	{
		for(int i = 0; i < 4; i++)
		{
			if(keys[i] == key)
				return;
			if(keys[i] == -1)
			{
				keys[i] = key;
				return;	
			}	
		}
	}
	//Release key	
	else if(action == GLFW_RELEASE)
		for(int i = 0; i < 4; i++)
			if(keys[i] == key)
				keys[i] = -1;	
}

int checkKey(int key)
{
	for(int i = 0; i < 4; i++)
		if(keys[i] == key)
			return 1; //Found key
	return 0; //Didn't find key
}

void handleWinResize(GLFWwindow *win, int newWidth, int newHeight)
{
	glViewport(0, 0, newWidth, newHeight);
	useProgram(getShader(DEFAULT_SHADER));
	glUniform2f(getUniform(DEFAULT_UNIFORM_SCREEN_DIMENSIONS),
				(float)newWidth,
				(float)newHeight);
	useProgram(getShader(BACKGROUND_SHADER));
	glUniform2f(getUniform(BACKGROUND_UNIFORM_SCREEN_DIMENSIONS),
				(float)newWidth,
				(float)newHeight);
	useProgram(getShader(ATTACK_ANIMATION_SHADER));
	glUniform2f(getUniform(ATTACK_UNIFORM_SCREEN_DIMENSIONS),
				(float)newWidth,
				(float)newHeight);

	winWidth = newWidth;
	winHeight = newHeight;
}

int getWindowWidth()
{
	return winWidth;
}

int getWindowHeight()
{
	return winHeight;
}
