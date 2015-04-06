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

//Creates the terrain from the set height map data and returns the model
Model* GenerateTerrain();

//Returns the most recent generated terrain
Model* GetTerrainModel();

//Calculates the face normal of a triangle
vec3 CalculateTriangleNormal(vec3 p1, vec3 p2, vec3 p3);

//Returns a new vector from a start index (useful instead of doing three calls to get a vertex)
vec3 GetVectorFromStartIndex(int idx);

//Get height at a certain point in the terrain (unscaled)
float GetHeight(GLfloat x, GLfloat z);

//Is the given point in the map? (unscaled)
int IsInMap(int x, int z);