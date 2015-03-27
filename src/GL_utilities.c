// GL utilities, bare essentials

//#define GL3_PROTOTYPES


#include "GL_utilities.h"

// From ShaderLoader

char* readFile(char *file)
{
	FILE *fptr;
	long length;
	char *buf;
	
	fptr = fopen(file, "rb"); /* Open file for reading */
	if (!fptr) /* Return NULL on failure */
		return NULL;
	fseek(fptr, 0, SEEK_END); /* Seek to the end of the file */
	length = ftell(fptr); /* Find out how many bytes into the file we are */
	buf = (char*)malloc(length+1); /* Allocate a buffer for the entire length of the file and a null terminator */
	fseek(fptr, 0, SEEK_SET); /* Go back to the beginning of the file */
	fread(buf, length, 1, fptr); /* Read the contents of the file in to the buffer */
	fclose(fptr); /* Close the file */
	buf[length] = 0; /* Null terminator */
	
	return buf; /* Return the buffer */
}


char* readFileOBS(char * filename)
{
	char * data;
	FILE *theFile;
	char c;
	long howMuch;
	
	// Get file length
	theFile = fopen(filename, "rb");
	if (theFile == NULL)
	{
		printf("%s not found\n", filename);
		return NULL;
	}
	howMuch = 0;
	c = 0;
	while (c != EOF)
	{
		c = getc(theFile);
		howMuch++;
	}
	fclose(theFile);

	printf("%ld bytes\n", howMuch);
	
	// Read it again
	data = (char *)malloc(howMuch);
	theFile = fopen(filename, "rb");
	fread(data, howMuch-1,1,theFile);
	fclose(theFile);
	data[howMuch-1] = 0;

//	printf("%s\n-----\n", data);
	printf("%s loaded from disk\n", filename);

	return data;
}

// Infolog: Show result of shader compilation
void printShaderInfoLog(GLuint obj)
{
	GLint infologLength = 0;
	GLint charsWritten  = 0;
	char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
		free(infoLog);
	}
}

void printProgramInfoLog(GLuint obj)
{
	GLint infologLength = 0;
	GLint charsWritten  = 0;
	char *infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
		free(infoLog);
	}
}

// Compile a shader, return reference to it
GLuint compileShaders(const char *vs, const char *fs)
{
	GLuint v,f,p;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(v, 1, &vs, NULL);
	glShaderSource(f, 1, &fs, NULL);
	glCompileShader(v);
	glCompileShader(f);
	p = glCreateProgram();
	glAttachShader(p,v);
	glAttachShader(p,f);
	glLinkProgram(p);
	glUseProgram(p);
	
	printShaderInfoLog(v);
	printShaderInfoLog(f);
	printProgramInfoLog(p);
	
	return p;
}

GLuint loadShaders(const char *vertFileName, const char *fragFileName)
// TO DO: Add geometry shader support
{
	// const 
	char *vs, *fs;
	GLuint p;
	
	vs = readFile((char *)vertFileName);
	fs = readFile((char *)fragFileName);
        if ((vs==NULL)||(fs==NULL)) printf("Loading shaders failed.\n");
	
	p = compileShaders(vs, fs);
	if (vs != NULL) free(vs);
	if (fs != NULL) free(fs);
	return p;
}

// End of ShaderLoader


void dumpInfo(void)
{
   printf ("Vendor: %s\n", glGetString (GL_VENDOR));
   printf ("Renderer: %s\n", glGetString (GL_RENDERER));
   printf ("Version: %s\n", glGetString (GL_VERSION));
   printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));
   printError ("dumpInfo");
}

/* report GL errors, if any, to stderr */
void printError(const char *functionName)
{
   GLenum error;
   while (( error = glGetError() ) != GL_NO_ERROR)
   {
	  fprintf (stderr, "GL error 0x%X detected in %s\n", error, functionName);
   }
}
