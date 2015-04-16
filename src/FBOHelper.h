#include "GL_utilities.h"

typedef struct FBOData
{
	GLuint color, depth, fbo;
	int width, height;
} FBOData;

FBOData* InitFBO(int width, int height);