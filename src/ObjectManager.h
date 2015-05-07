#include "WorldObject.h"
#include "Tilemap.h"


void SetupObjectManager(float * deltaTime, float * modelView, float * camMatrix, float * projectionMatrix);
void DrawObjectManager(vec3 sun);
void GenerateObjects(Tilemap* tilemap);