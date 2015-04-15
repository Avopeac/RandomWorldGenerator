#include "GL_utilities.h"

typedef struct FBOData
{
	GLuint texId;
	GLuint fb;
	GLuint rb;
	int width, height;
} FBOData;

void InitFBO(void);
FBOData GetFBOData(void);