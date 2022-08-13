#ifndef INCLUDE_GL_FUNC
typedef struct
{
	unsigned int vertBuffer, texCoordBuffer;
} GLBufferObj;

//Create a square vertex buffer
unsigned int createSquareVertBuffer(void);
//Bind a vertex buffer
void bindVert(unsigned int vertBuffer);
//Texture coordinates
unsigned int createSquareTexCoords(void);
void bindTex(unsigned int texCoords);

GLBufferObj* createSquare(void);
void bindGLObj(GLBufferObj *obj);

//Shaders
unsigned int readShaderFile(const char *path, int type);
unsigned int createShaderProgram(const char *vertPath, const char *fragPath);

//Output any OpenGL errors for debugging
void getGLErrors(void);
void printShaderErrors(unsigned int shader);

//Textures
unsigned int loadTexture(const char *texPath);
#endif

#define INCLUDE_GL_FUNC
