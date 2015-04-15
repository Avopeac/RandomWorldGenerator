#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "loadobj.h"
#include "LoadTGA.h"

//Standard settings
#define STANDARD_MAP_SCALE 4.0

//The height and width of this terrain
int terrainWidth;
int terrainHeight;

//Set terrain scale
void SetTerrainScale(float scale);

//Get terrain scale
float GetTerrainScale();

//Set the height map to generate terrain from
void SetHeightMapTextureData(char *file);

void SetupHeightMapTerrain(GLfloat *deltaTime, mat4 *modelWorld, mat4 *worldView, mat4 *projectionMatrix);

void DrawHeightMapTerrain(vec3 sun);

//Creates the terrain from the set height map data and returns the model
void GenerateTerrain();

//Returns the most recent generated terrain
Model* GetTerrainModel();

//Returns a new vector from a start index (useful instead of doing three calls to get a vertex)
vec3 GetVectorFromStartIndex(int idx);

//Get height at a certain point in the terrain (unscaled)
float GetHeight(GLfloat x, GLfloat z);

//Is the given point outside the map? (unscaled)
int IsOutsideMap(int x, int z);