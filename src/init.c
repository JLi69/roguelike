#include "roguelike.h"
#include <stdio.h>
#include "event.h"
#include <glad/glad.h>
#include <stdlib.h>

static GLBufferObj* buffers[BUFFER_COUNT];
static unsigned int shaders[SHADER_COUNT];
static unsigned int textures[TEXTURE_COUNT]; 
static int uniforms[UNIFORM_COUNT];

void init(void)
{
	//Square vertex buffer	
	buffers[SQUARE] = createSquare();
	//Shaders
	shaders[DEFAULT_SHADER] = createShaderProgram("res/shaders/vert.glsl", "res/shaders/texture-frag.glsl");	
	shaders[BACKGROUND_SHADER] = createShaderProgram("res/shaders/background-vert.glsl", "res/shaders/texture-frag.glsl");	
	//Set up uniforms
	uniforms[DEFAULT_UNIFORM_OFFSET] = glGetUniformLocation(getShader(DEFAULT_SHADER), "uOffset");
	uniforms[DEFAULT_UNIFORM_SCALE] = glGetUniformLocation(getShader(DEFAULT_SHADER), "uScale");
	uniforms[DEFAULT_UNIFORM_SCREEN_DIMENSIONS] = glGetUniformLocation(getShader(DEFAULT_SHADER), "uScreenDimensions");
	uniforms[DEFAULT_UNIFORM_DAMAGE_COOLDOWN] = glGetUniformLocation(getShader(DEFAULT_SHADER), "uDamageCooldown");
	uniforms[DEFAULT_UNIFORM_TEX_SIZE] = glGetUniformLocation(getShader(DEFAULT_SHADER), "uTexSize");
	uniforms[DEFAULT_UNIFORM_TEX_FRAC] = glGetUniformLocation(getShader(DEFAULT_SHADER), "uTexFrac");
	uniforms[DEFAULT_UNIFORM_TEX_OFFSET] = glGetUniformLocation(getShader(DEFAULT_SHADER), "uTexOffset");

	uniforms[BACKGROUND_UNIFORM_SCALE] = glGetUniformLocation(getShader(BACKGROUND_SHADER), "uScale");
	uniforms[BACKGROUND_UNIFORM_SCREEN_DIMENSIONS] = glGetUniformLocation(getShader(BACKGROUND_SHADER), "uScreenDimensions");
	uniforms[BACKGROUND_UNIFORM_DAMAGE_COOLDOWN] = glGetUniformLocation(getShader(BACKGROUND_SHADER), "uDamageCooldown");
	uniforms[BACKGROUND_UNIFORM_TEX_SIZE] = glGetUniformLocation(getShader(BACKGROUND_SHADER), "uTexSize");
	uniforms[BACKGROUND_UNIFORM_TEX_FRAC] = glGetUniformLocation(getShader(BACKGROUND_SHADER), "uTexFrac");
	uniforms[BACKGROUND_UNIFORM_TEX_OFFSET] = glGetUniformLocation(getShader(BACKGROUND_SHADER), "uTexOffset");

	//Textures	
	//tex = loadTexture("res/textures/test.png");	
	textures[TILE_TEXTURE_MAP] = loadTexture("res/textures/tiles.png");
	textures[SPRITE_TEXTURE_MAP] = loadTexture("res/textures/sprites.png");
	textures[ICON_TEXTURE_MAP] = loadTexture("res/textures/icons.png");

	getGLErrors();
}

void terminate(void)
{
	for(int i = 0; i < BUFFER_COUNT; i++)
		free(buffers[i]);
}

unsigned int getShader(unsigned int shader)
{
	return shaders[shader];
}

unsigned int getTexture(unsigned int texture)
{
	return textures[texture];
}

int getUniform(unsigned int uniform)
{
	return uniforms[uniform];
}

GLBufferObj* getBuffer(unsigned int buffer)
{
	return buffers[buffer];
}

GLFWwindow* initWindow(void)
{
	//Attempt to initialize glfw
	if(!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW!\n");
		return NULL;
	}

	//Attept to initialize the window
	GLFWwindow* win = glfwCreateWindow(960, 540, "Roguelike", NULL, NULL);
	//Failed to create window
	if(!win)
	{
		fprintf(stderr, "Failed to create window!\n");
		glfwTerminate();	
		return NULL;
	}
	glfwMakeContextCurrent(win);
	
	//Attempt to initialize glad
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		fprintf(stderr, "Failed to load glad!\n");	
		glfwTerminate();
		return NULL;
	}

	//Set up key input
	glfwSetKeyCallback(win, onKeyPress);
	//Set up how the window should respond to resizing
	glfwSetWindowSizeCallback(win, handleWinResize);

	return win;
}
