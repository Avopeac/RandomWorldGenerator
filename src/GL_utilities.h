#ifndef _GL_UTILITIES_
#define _GL_UTILITIES_

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
#else
    #include <GL/glew.h>
	#include <GL/glut.h>
 
    #include <stdlib.h>
    #include <stdio.h>
    #include <stddef.h>
    #include <string.h>
    
	//#include <GL/gl.h>
#endif

char* readFile(char *file);
char* readFileOBS(char * filename);
void printShaderInfoLog(GLuint obj);
void printProgramInfoLog(GLuint obj);
GLuint compileShaders(const char *vs, const char *fs);

void printError(const char *functionName);
GLuint loadShaders(const char *vertFileName, const char *fragFileName);
void dumpInfo(void);

#endif
