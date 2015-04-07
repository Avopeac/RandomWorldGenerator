// Lab 4, terrain generation

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	#include "MicroGlut.h"
	// Linking hint for Lightweight IDE
	// uses framework Cocoa
#endif

#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"
#include "FirstPersonCamera.h"
#include "HeightMapTerrain.h"
#include "Resources.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define WINDOW_TITLE "Random World Generator"

mat4 projectionMatrix;
CameraData camData;

// Reference to shader program
GLuint program;
GLuint tex1, tex2;
GLfloat elapsedTime;
GLfloat deltaTime;

void init(void)
{
	
	//Set up controls
	InitPassiveMotionFunc();
	InitKeyboardFunc();

	// GL inits
	dumpInfo();
	glClearColor(0.2f, 0.2f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");

	projectionMatrix = frustum(-0.1f, 0.1f, -0.1f, 0.1f, 0.2f, 100.0f);
	SetCameraStartPosition(20, 0, 20);

	// Load and compile shader
	program = loadShaders(TERRAIN_VERT_SHADER, TERRAIN_FRAG_SHADER);
	glUseProgram(program);
	printError("init shader");
	
	//Upload to GPU
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniform1i(glGetUniformLocation(program, "tex"), 0); // Texture unit 0
	LoadTGATextureSimple(MASKROS_512_TEXTURE, &tex1);

	//Set up terrain model
	SetHeightMapTextureData(TERRAIN_FFT_TEXTURE);
	GenerateTerrain();
}

void display(void)
{
	mat4 total, modelView, camMatrix;
	
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	printError("pre display");
	
	glUseProgram(program);


	// Build matrix
	camData = GetCameraData();
	//camMatrix = lookAtv(camData.pos, camData.lookAt, camData.normal);

	camMatrix = Mult(Mult(Rx(-camData.rot.y), Ry(camData.rot.x)),
		T(-camData.pos.x, -camData.pos.y, -camData.pos.z));

	//Debug print
	//PrintCameraData();

	modelView = IdentityMatrix();
	total = Mult(camMatrix, modelView);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	
	// Bind Our Texture tex1
	glBindTexture(GL_TEXTURE_2D, tex1);		
	DrawModel(GetTerrainModel(), program, "inPosition", "inNormal", "inTexCoord");

	printError("display 2");

	CameraMoveAround(MOVE_SPEED, MOUSE_SENSITIVITY, deltaTime);
	SetCameraHeight(GetHeight(camData.pos.x * GetTerrainScale(),
		camData.pos.z * GetTerrainScale()));
	
	glutSwapBuffers();
}

void timer(int i)
{
	GLfloat t = (GLfloat)glutGet(GLUT_ELAPSED_TIME);
	deltaTime = t - elapsedTime;
	elapsedTime = t;

	glutTimerFunc(20, &timer, i);
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);

	//If using MicroGLUT?
	//glutInitContextVersion(3, 2);

	glutInitWindowSize (WINDOW_WIDTH, WINDOW_HEIGHT);
	SetWindowSizeForMovement (WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow (WINDOW_TITLE);
	glutSetCursor(GLUT_CURSOR_NONE);
	glutDisplayFunc(display);

	if (GLEW_OK != glewInit())
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		printf("glewInit failed, aborting.\n");
		exit (1);
	}

	init ();
	glutTimerFunc(20, &timer, 0);
	glutMainLoop();
	exit(0);
}