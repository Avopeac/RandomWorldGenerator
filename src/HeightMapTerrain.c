#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "loadobj.h"
#include "LoadTGA.h"
#include "HeightMapTerrain.h"

//To get the 1-ring neighborhood of a point
#define NEIGHBOR 6
#define NEIGHBOR_DIR 4
int direction[NEIGHBOR][NEIGHBOR_DIR] = 
  {
    {1,0,0,1}, {0,1,-1,1}, {-1,1,-1,0},
    {-1,0,0,-1}, {0,-1,1,-1}, {1,-1,1,0},
  };
  
//The terrain model
Model* terrainModel;

//The height map texture data
TextureData terrainTexture;

//The scale to sample with
float terrainScale = STANDARD_MAP_SCALE;

//Generates the terrain model based on scale and the set height map texture data
Model* GenerateTerrain()
{
  unsigned int vertexCount = terrainTexture.width * terrainTexture.height;
  unsigned int triangleCount = (terrainTexture.width-1) * (terrainTexture.height-1) * 2;
  unsigned int x, z;
  
  GLfloat *vertexArray = (GLfloat *)malloc(sizeof(GLfloat) * 3 * vertexCount);
  GLfloat *normalArray = (GLfloat *)malloc(sizeof(GLfloat) * 3 * vertexCount);
  GLfloat *texCoordArray = (GLfloat *)malloc(sizeof(GLfloat) * 2 * vertexCount);
  GLuint *indexArray = (GLuint *)malloc(sizeof(GLuint) * triangleCount * 3);
  
  for (x = 0; x < terrainTexture.width; x++)
    for (z = 0; z < terrainTexture.height; z++)
      {
	vertexArray[(x + z * terrainTexture.width)*3 + 0] = x / terrainScale;
	vertexArray[(x + z * terrainTexture.width)*3 + 1] = terrainTexture.imageData[(x + z * terrainTexture.width) * (terrainTexture.bpp/8)] / 100.0f;
	vertexArray[(x + z * terrainTexture.width)*3 + 2] = z / terrainScale;

	normalArray[(x + z * terrainTexture.width)*3 + 0] = 0.0;
	normalArray[(x + z * terrainTexture.width)*3 + 1] = 1.0;
	normalArray[(x + z * terrainTexture.width)*3 + 2] = 0.0;

	texCoordArray[(x + z * terrainTexture.width)*2 + 0] = (GLfloat)x; // (float)x / tex->width;
	texCoordArray[(x + z * terrainTexture.width)*2 + 1] = (GLfloat)z; // (float)z / tex->height;
      }

	/*
  for (x = 0; x < terrainTexture.width; x++)
    for(z = 0; z < terrainTexture.height; z++)
      {
     
	int idx = (x + z * terrainTexture.width) * 3;
	vec3 normal = SetVector(0,1,0);
	vec3 v0 = GetVectorFromStartIndex(idx);

	int i;
	for (i = 0; i < NEIGHBOR; i++)
	  {

	    //Neighbor directions
	    int neighborX1 = x + direction[i][0];
	    int neighborZ1 = z + direction[i][1];
	    int neighborX2 = x + direction[i][2];
	    int neighborZ2 = z + direction[i][3];
	    
	    if (IsInMap(neighborX1, neighborZ1) || IsInMap(neighborX2, neighborZ2))
	      {
		int ti1 = (neighborX1 + neighborZ1 * terrainTexture.width) * 3;
		int ti2 = (neighborX2 + neighborZ2 * terrainTexture.width) * 3;
		
		vec3 v1 = GetVectorFromStartIndex(ti1);
		vec3 v2 = GetVectorFromStartIndex(ti2); 

		vec3 n = CalculateTriangleNormal(v0, v1, v2); 

		normal = VectorAdd(normal, n);
	      }
	  }

	normal = Normalize(normal);

	normalArray[idx + 0] = normal.x;
	normalArray[idx + 1] = normal.y;
	normalArray[idx + 2] = normal.z;
      } */

  for (x = 0; x < terrainTexture.width-1; x++)
    for (z = 0; z < terrainTexture.height-1; z++)
      { 
	//Triangle 1
	indexArray[(x + z * (terrainTexture.width-1))*6 + 0] = x + z * terrainTexture.width;
	indexArray[(x + z * (terrainTexture.width-1))*6 + 1] = x + (z + 1) * terrainTexture.width;
	indexArray[(x + z * (terrainTexture.width-1))*6 + 2] = x + 1 + z * terrainTexture.width;
      
	//Triangle 2
	indexArray[(x + z * (terrainTexture.width-1))*6 + 3] = x + 1 + z * terrainTexture.width;
	indexArray[(x + z * (terrainTexture.width-1))*6 + 4] = x + (z + 1) * terrainTexture.width;
	indexArray[(x + z * (terrainTexture.width-1))*6 + 5] = x + 1 + (z + 1) * terrainTexture.width;
      }

  // End of terrain generation
  // Create Model and upload to GPU:
  terrainModel = LoadDataToModel(
				 vertexArray,
				 normalArray,
				 texCoordArray,
				 NULL,
				 indexArray,
				 vertexCount,
				 triangleCount*3);

  return terrainModel;
}

//Set the height map texture data to sample from
void SetHeightMapTextureData(char* file)
{
	LoadTGATextureData(file, &terrainTexture);

	terrainWidth = (int)terrainTexture.width;
	terrainHeight = (int)terrainTexture.height;
}

//Getter for terrain model (call only after generation)
Model* GetTerrainModel()
{	
	return terrainModel;
}

//Setter for terrain scale (call before generation)
void SetTerrainScale(float scale)
{
	terrainScale = scale;
}

//Returns the current terrain scale
float GetTerrainScale()
{
	return terrainScale;
}

//Calculate normal of 3 point face. 
vec3 CalculateTriangleNormal(vec3 p1, vec3 p2, vec3 p3)
{
  vec3 v1 = VectorSub(p2, p1);    
  vec3 v2 = VectorSub(p3, p1);
  vec3 cross = CrossProduct(v2, v1);

  return cross;
}

//Get vertex vector from a start index (reduces 3 calls to one when getting vertices)
vec3 GetVectorFromStartIndex(int idx)
{
  GLfloat x = terrainModel->vertexArray[idx + 0];
  GLfloat y = terrainModel->vertexArray[idx + 1];
  GLfloat z = terrainModel->vertexArray[idx + 2];
  
  return SetVector(x,y,z);
}

//Get height of a specific point on the terrain (unscaled)
float GetHeight(GLfloat x, GLfloat z)
{
  int tx = (int)floor(x);
  int tz = (int)floor(z);

  vec3 v0 = GetVectorFromStartIndex((tx + tz * terrainWidth) * 3);
  vec3 v1 = GetVectorFromStartIndex((tx + 1 + tz * terrainWidth) * 3);
  vec3 v2 = GetVectorFromStartIndex((tx + (tz + 1) * terrainWidth) * 3);
  vec3 v3 = GetVectorFromStartIndex((tx + 1 + (tz + 1) * terrainWidth) * 3);

  float h = 0.0f;

  float t0 = x - tx;
  float t1 = z - tz;

  // If first triangle else second triangle in quad
  if (t0 + t1 < 1)
   {  
     h = v0.y;
     h += (v1.y - v0.y) * t0;
     h += (v2.y - v0.y) * t1;
   }
 else
   {  
     h = v3.y;  
     h += (v2.y - v3.y) * (1.0f - t0);
     h += (v1.y - v3.y) * (1.0f - t1);
   }

  return h;
}

//Simple check if the point is in the map (unscaled)
int IsInMap(int x, int z)
{
  return !(x < 0 || x > terrainWidth || z < 0 || z > terrainHeight) ? 1 : 0;
}


