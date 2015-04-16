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
#include "DayNightCycle.h"
#include "WaterSource.h"
#include "FBOHelper.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define WINDOW_TITLE "Random World Generator"

mat4 projectionMatrix, total, modelView, camMatrix;
CameraData camData;
SolarData sData;

GLfloat elapsedTime;
GLfloat deltaTime;

FBOData *rt;

//This will be removed later and generated procedurally
WaterSource* water;

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

	rt = InitFBO(WINDOW_WIDTH, WINDOW_HEIGHT);

	printError("GL inits");

	modelView = IdentityMatrix();
	camMatrix = IdentityMatrix();
	projectionMatrix = frustum(-0.1f, 0.1f, -0.1f, 0.1f, 0.2f, 100.0f);
	SetCameraStartPosition(20, 0, 20);

	//Set up terrain model
	SetupHeightMapTerrain(&deltaTime, &modelView, &camMatrix, &projectionMatrix);
	SetHeightMapTextureData(TERRAIN_FFT_TEXTURE);
	GenerateTerrain();

	//Set up day night cycle
	SetupDayNightCycle(&deltaTime, &modelView, &camMatrix, &projectionMatrix);
	InitDayNightCycle(2015, 05, 30, 70000, 1.0f,
		(float)(LATITUDE_STHLM_SWEDEN * M_PI / 180.0f),
		(float)(LONGITUDE_STHLM_SWEDEN * M_PI / 180.0f), 2);

	//Set up a water source
	SetupWaterSources(&deltaTime, &modelView, &camMatrix, &projectionMatrix);
	water = GenerateWaterSource(
		SetVector(20, -6.5f, 20),
		240, 240,
		5.0f, 4.0f, 20.0f,
		0.05f, 0.1f, 0.03f,
		0.2f, -1.5f, 2.5f,
		SetVector(1,0,1), SetVector(-1,0,-1), SetVector(1, 0, -1),
		GetTerrainModel());
}

void display(void)
{	
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	printError("pre display");

	CameraMoveAround(MOVE_SPEED, MOUSE_SENSITIVITY, deltaTime);
	SetCameraHeight(GetHeight(camData.pos.x * GetTerrainScale(),
		camData.pos.z * GetTerrainScale()) + CAMERA_HEIGHT);

	// Build matrix
	camData = GetCameraData();
	//camMatrix = Mult(Mult(Rx(-camData.rot.y), Ry(camData.rot.x)),
	//	T(-camData.pos.x, -camData.pos.y, -camData.pos.z));

	sData = GetSolarData();

	glBindFramebuffer(GL_FRAMEBUFFER, rt->fbo);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,rt->width, rt->height);
	glDisable(GL_TEXTURE_2D);
	camMatrix = Mult(Mult(Rz(M_PI), Ry(camData.rot.x)),
		T(-camData.pos.x, -camData.pos.y, -camData.pos.z));
	DrawDayNightCycle();
	DrawHeightMapTerrain(sData.position);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	camMatrix = Mult(Mult(Rx(-camData.rot.y), Ry(camData.rot.x)),
		T(-camData.pos.x, -camData.pos.y, -camData.pos.z));

	DrawDayNightCycle();
	DrawHeightMapTerrain(sData.position);
	DrawWaterSource(water, sData.position, sData.zenithAngle, camData.pos, rt->color);

	printError("display 2");
	
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
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

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