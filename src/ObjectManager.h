#include "WorldObject.h"
#include "Tilemap.h"
#include "FirstPersonCamera.h"


void SetupObjectManager(float * deltaTime, mat4 *modelWorld, mat4 *worldView, mat4 *projectionMatrix);
void DrawObjectManager(vec3 sun, CameraData cam);
void GenerateObjects(Tilemap* tilemap);