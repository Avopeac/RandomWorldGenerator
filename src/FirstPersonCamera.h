#include "GL_utilities.h"
#include "VectorUtils3.h"

//Standard settings
#define MOUSE_SENSITIVITY 0.1f
#define MOVE_SPEED 0.005f
#define CAMERA_HEIGHT 1.0f

//Controls
#define CONTROL_COUNT 5
#define FORWARD 'w'
#define BACKWARD 's'
#define LEFT 'a'
#define RIGHT 'd'
#define EXIT 27

//The camera data struct has members for look at data
typedef struct CameraData 
{
  vec3 pos;
  vec3 lookAt;
  vec3 normal;
  vec3 rot;
} CameraData;

//The mouse data struct is used when calculating mouse movement
typedef struct MouseData
{
  float dy;
  float dx;
  float x;
  float y;
} MouseData;

//Sets the window size that is currently used making mouse movement correct
void SetWindowSizeForMovement(int x, int y);
void SetWorldSizeForMovement(int width, int height);
void SetCameraStartPosition(float x, float y, float z);

//Call every update to move camera with W,A,S,D and mouse
void CameraMoveAround(float speed, float sensitivity, float deltaTime);

//Sets the height of the camera, this might correspond to player height or closeness to the ground
void SetCameraHeight(float height);

//Call this in main function to initialize mouse movement code
void InitPassiveMotionFunc();

//Call this in main function to initialize keyboard movement code
void InitKeyboardFunc();

//Print the currently pressed keys in the map
void PrintKeys();

//Prints camera matrix
void PrintCameraData();

//Prints mouse movement
void PrintMouseData();

//Get the current camera movement data
MouseData GetMouseData();
CameraData GetCameraData();

