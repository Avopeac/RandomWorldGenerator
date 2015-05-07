#include "WorldObject.h"
#include "Tilemap.h"


void SetupObjectManager(float * deltaTime, mat4 *modelWorld, mat4 *worldView, mat4 *projectionMatrix);
void DrawObjectManager(vec3 sun);
void GenerateObjects(Tilemap* tilemap);