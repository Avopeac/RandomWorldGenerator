
// Includes vary a bit with platforms.
// MS Windows needs GLEW or glee.
// Mac uses slightly different paths.
#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	#include "MicroGLUT.h"
	// A little linking trick for Lightweight IDE
	#if 0
		#import <Cocoa/Cocoa.h>
	#endif
#endif
#include "GL_utilities.h"
//#include "Vertices.h"

// Globals
// Data would normally be read from files
GLfloat vertices[] = {	-0.5f,-0.5f,0.0f,
						-0.5f,0.5f,0.0f,
						0.5f,-0.5f,0.0f };

// NEW
GLfloat rotationMatrix[] = {	1.0f, -0.0f, 0.0f, 0.0f,
						0.0f, 1.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 1.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 1.0f };
						
//Vertices *vertex;

// vertex array object
unsigned int vertexArrayObjID;


void init(void)
{
	// two vertex buffer objects, used for uploading the
	unsigned int vertexBufferObjID;
	// Reference to shader program
	GLuint program;
    dumpInfo(); 
	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_DEPTH_TEST);
	printError("GL inits");

	// Load and compile shader
	program = loadShaders("minimal.vert", "minimal.frag");
	glUseProgram(program);
	printError("init shader");
	
	// Upload gemoetry to the GPU:
	
	// Allocate and activate Vertex Array Object
	glGenVertexArrays(1, &vertexArrayObjID);
	glBindVertexArray(vertexArrayObjID);
	// Allocate Vertex Buffer Objects
	glGenBuffers(1, &vertexBufferObjID);
	
	// VBO for vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, 9*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(program, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(glGetAttribLocation(program, "in_Position"));

	// End of upload of geometry

// NEW
	glUniformMatrix4fv(glGetUniformLocation(program, "myMatrix"), 1, GL_FALSE, rotationMatrix);
	
	printError("init arrays");
}

void display(void)
{
	printError("pre display");

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vertexArrayObjID);	// Select VAO
	glDrawArrays(GL_TRIANGLES, 0, 3);	// draw object
	
	printError("display");
	
	glFlush();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutCreateWindow ("GL3 rotation example");
	glutDisplayFunc(&display); 

	if (GLEW_OK != glewInit())
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		printf("glewInit failed, aborting.\n");
		exit (1);
	}
	printf("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	init ();
	glutMainLoop();
}
