#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "FirstPersonCamera.h"
#include <GL\freeglut.h>

//Window settings
int windowWidth;
int windowHeight;

//World settings
int worldWidth;
int worldHeight;

//Camera movement data
MouseData mData;
CameraData cData;

vec3 camDirection, camDirectionRight, camForward;

//Controls
char controlMap[CONTROL_COUNT] = {FORWARD, LEFT, BACKWARD, RIGHT, EXIT};
int controlBitmap[CONTROL_COUNT] = {0,0,0,0,0};

//Set camera height 
void SetCameraHeight(float height)
{
	cData.pos.y = height + CAMERA_HEIGHT;
}

void SetCameraStartPosition(float x, float y, float z)
{
	cData.pos.x = x;
	cData.pos.y = y;
	cData.pos.z = z;
}

//Set window settings 
void SetWindowSizeForMovement(int x, int y)
{
  windowWidth = x;
  windowHeight = y;
}

void SetWorldSizeForMovement(int width, int height)
{
	worldWidth = width;
	worldHeight = height;
}

//Declare a callback function
void MouseCallback(int x, int y);
void KeyboardCallback(unsigned char key, int x, int y);
void KeyboardUpCallback(unsigned char key, int x, int y);

//Set call back function for mouse movement
void InitPassiveMotionFunc()
{
  void (*func)(int, int);
  func = &MouseCallback;
  glutPassiveMotionFunc(func);
}

void InitKeyboardFunc()
{
	void (*func)(unsigned char, int, int);

	//Key down
	func = &KeyboardCallback;
	glutKeyboardFunc(func);

	//Key up
	func = &KeyboardUpCallback;
	glutKeyboardUpFunc(func);
}

// Mouse look around for camera
void MouseCallback(int x, int y)
{
  int center_x = windowWidth / 2;
  int center_y = windowHeight / 2;

  mData.dx = (center_x - x) / ((float)windowWidth); 
  mData.dy = (center_y - y) / ((float)windowHeight);
}

void KeyboardCallback(unsigned char key, int x, int y)
{
	int i;

	for (i = 0; i < CONTROL_COUNT; ++i)
	{
		if (key == controlMap[i])
		{
			controlBitmap[i] = 1;
		}
	}
}

void KeyboardUpCallback(unsigned char key, int x, int y)
{
	int i;

	for (i = 0; i < CONTROL_COUNT; ++i)
	{
		if (key == controlMap[i])
		{
			controlBitmap[i] = 0;
		}
	}
}

// Move around in the world. 
//THIS IMPLEMENTATION DOES NOT MAKE USE OF LOOKAT FUNCTIONS, ONLY ROTATION AND POSITION I.E YOU NEED MULTIPLY MATRICES YOURSELF.
void CameraMoveAround(float speed, float sensitivity, float deltaTime)
{

  mData.x += mData.dx * sensitivity * deltaTime;
  mData.y += mData.dy * sensitivity * deltaTime;

  //Clamp y-axis rotation
  if (mData.y >= 0.9999f)
    {
      mData.y = 0.9999f;
    }
  
  if (mData.y <= -0.9999f)
    {
      mData.y = -0.9999f;
    }

  cData.rot.x = 2.0f * M_PI * mData.x;
  cData.rot.y = M_PI / 2.0f * mData.y;

  camDirectionRight = SetVector((GLfloat)(sin(cData.rot.x - M_PI / 2.0f)),
	  0.0f,
	  (GLfloat)(cos(cData.rot.x - M_PI / 2.0f)));

  camForward = SetVector((GLfloat)(sin(cData.rot.x)),
	  0.0f,
	  (GLfloat)(cos(cData.rot.x)));

   // W is pressed
   if (controlBitmap[0] == 1)
   {
	   cData.pos = VectorSub(cData.pos, ScalarMult(camForward, speed * deltaTime));
   }

   // A is pressed
   if (controlBitmap[1] == 1)
   {
	   cData.pos = VectorAdd(cData.pos, ScalarMult(camDirectionRight, speed * deltaTime));
   }

   // S is pressed
   if (controlBitmap[2] == 1)
   {
	   cData.pos = VectorAdd(cData.pos, ScalarMult(camForward, speed * deltaTime));
   }

   // D is pressed
   if (controlBitmap[3] == 1)
   {
	   cData.pos = VectorSub(cData.pos, ScalarMult(camDirectionRight, speed * deltaTime));
   }

   // Esc is pressed
   if (controlBitmap[4] == 1)
   {
	   exit(0);
   }

  //Warp pointer to center of screen here, if this is done in callback function then it will cause more callbacks...
  glutWarpPointer(windowWidth / 2, windowHeight / 2);
}

MouseData GetMouseData()
{
  return mData;
}

CameraData GetCameraData()
{
  return cData;
}

//Debug print for keys
void PrintKeys()
{
	int i;

	for (i = 0; i < CONTROL_COUNT; ++i)
	{
		fprintf(stderr, "%c = %i\t", controlMap[i], controlBitmap[i]);
	}
	fprintf(stderr, "\n");
}

//Debug print for camera
void PrintCameraData()
{
	fprintf(stderr, "Camera position = %f %f %f\n", cData.pos.x, cData.pos.y, cData.pos.z);
	fprintf(stderr, "Camera look at = %f %f %f\n", cData.lookAt.x, cData.lookAt.y, cData.lookAt.z);
	fprintf(stderr, "Camera normal = %f %f %f\n", cData.normal.x, cData.normal.y, cData.normal.z);
}

//Debug print for mouse
void PrintMouseData()
{
	fprintf(stderr, "Mouse x y = %f %f", mData.x, mData.y);
	fprintf(stderr, "Mouse dx dy = %f %f", mData.dx, mData.dy);
}