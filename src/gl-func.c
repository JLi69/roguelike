#include "gl-func.h"
#include <glad/glad.h>
#include <stdlib.h>
#include <stdio.h>
#include <SOIL2/SOIL2.h>

//Current shader program being used
static int currentShaderProgram = 0;

unsigned int createSquareVertBuffer(void)
{
	static const float squareVerts[] = 
	{
		 1.0f,  1.0f,
		-1.0f,  1.0f,
		 1.0f, -1.0f,

		 1.0f, -1.0f,
		-1.0f,  1.0f,
		-1.0f, -1.0f
	};

	unsigned int vertBufferId;
	glGenBuffers(1, &vertBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(squareVerts), squareVerts, GL_STATIC_DRAW);		

	return vertBufferId;
}

unsigned int createSquareTexCoords(void)
{
	static const float squareTexCoords[] = 
	{
		 1.0f,  1.0f,
		 0.0f,  1.0f,
		 1.0f,  0.0f,

		 1.0f,  0.0f,
		 0.0f,  1.0f,
		 0.0f,  0.0f
	};

	unsigned int texCoordId;
	glGenBuffers(1, &texCoordId);
	glBindBuffer(GL_ARRAY_BUFFER, texCoordId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(squareTexCoords), squareTexCoords, GL_STATIC_DRAW);		

	return texCoordId;
}

void bindVert(unsigned int vertBuffer)
{
	glBindBuffer(GL_ARRAY_BUFFER, vertBuffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
	glEnableVertexAttribArray(0);
}

void bindTex(unsigned int texCoords)
{
	glBindBuffer(GL_ARRAY_BUFFER, texCoords);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
	glEnableVertexAttribArray(1);
}

GLBufferObj* createSquare(void)
{
	GLBufferObj* square = (GLBufferObj*)malloc(sizeof(GLBufferObj));
	square->vertBuffer = createSquareVertBuffer();
	square->texCoordBuffer = createSquareTexCoords();
	return square;
}

void bindGLObj(GLBufferObj *obj)
{
	bindVert(obj->vertBuffer);
	bindTex(obj->texCoordBuffer);
}

void getGLErrors(void)
{
	GLenum glErr = glGetError();
	while(glErr != GL_NO_ERROR)
	{
		fprintf(stderr, "OpenGL Error: %d\n", glErr);
		glErr = glGetError();
	}
}

//Shaders
unsigned int readShaderFile(const char *path, int type)
{
	//ID of the shader source	
	unsigned int shaderSrcId = glCreateShader(type);

	FILE* srcFile = fopen(path, "r");
	//Can't open file	
	if(!srcFile)
	{
		fprintf(stderr, "File not found: %s\n", path);
		return 0; //Return 0 if shader is not found
	}

	char* src = (char*)malloc(4096); //Allocate 4 KB of memory for the file
	char* ptr = src; //Pointer to the current character	
	//Mark all the characters as spaces
	for(int i = 0; i < 4096; i++) src[i] = ' ';	
	//Read the file
	while((*ptr++ = getc(srcFile)) != EOF);
	//Null terminate the source code
	*--ptr = '\0';

	//printf("%s\n", src);

	//send the source code to OpenGL
	const char* shaderBegin = src;
	glShaderSource(shaderSrcId, 1, &shaderBegin, NULL); 

	fclose(srcFile);
	free(src);

	return shaderSrcId;
}

void printShaderErrors(unsigned int shader)
{	
	int compiled, type;	
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	glGetShaderiv(shader, GL_SHADER_TYPE, &type);

	//Did not compile!
	if(!compiled)
	{
		switch(type)
		{
		case GL_VERTEX_SHADER: fprintf(stderr, "Vertex shader failed to compile!\n"); break;
		case GL_FRAGMENT_SHADER: fprintf(stderr, "Fragment shader failed to compile!\n"); break;
		default: fprintf(stderr, "Shader failed to compile!\n"); break;
		}

		//Get all the error messages
		int errLen, chWritten;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errLen);
		char* msg = (char*)malloc(errLen);
		glGetShaderInfoLog(shader, errLen, &chWritten, msg);
		fprintf(stderr, "%s", msg);

		free(msg);
	}
}

unsigned int createShaderProgram(const char *vertPath, const char *fragPath)
{
	unsigned int vert = readShaderFile(vertPath, GL_VERTEX_SHADER),
				 frag = readShaderFile(fragPath, GL_FRAGMENT_SHADER);
	
	if(!vert || !frag)
		return 0;	

	unsigned int program = glCreateProgram();

	//Compile the shaders
	glCompileShader(vert);
	glCompileShader(frag);
	//Print out compiler errors
	printShaderErrors(vert);
	printShaderErrors(frag);

	glAttachShader(program, vert);
	glAttachShader(program, frag);
	glLinkProgram(program);	
	glValidateProgram(program);	

	//Print out the link status
	int linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if(!linked)
		fprintf(stderr, "Program failed to link!\n\n");

	glDetachShader(program, vert);
	glDetachShader(program, frag);	

	return program;
}

void useProgram(unsigned int program)
{
	glUseProgram(program);
	currentShaderProgram = program;
}

unsigned int getActiveProgram(void)
{
	return currentShaderProgram;
}

unsigned int loadTexture(const char *path)
{
	//Use SOIL2 to load the texture	
	unsigned int tex = SOIL_load_OGL_texture(path,
											 SOIL_LOAD_AUTO,
											 SOIL_CREATE_NEW_ID,
											 SOIL_FLAG_INVERT_Y);
	//Check if the image was loaded
	if(tex == 0)
	{
		fprintf(stderr, "Cannot find texture: %s\n", path);	
		return 0;
	}

	return tex;
}

void activateTexture(unsigned int tex, unsigned int binding)
{
	glActiveTexture(binding);
	glBindTexture(GL_TEXTURE_2D, tex);
}
