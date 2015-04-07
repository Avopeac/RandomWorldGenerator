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
void GenerateTerrain()
{
	unsigned int vertexCount = terrainTexture.width * terrainTexture.height;
	unsigned int triangleCount = (terrainTexture.width-1) * (terrainTexture.height-1) * 2;
	unsigned int x, z;

	GLfloat *vertexArray = (GLfloat *)malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *normalArray = (GLfloat *)malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *texCoordArray = (GLfloat *)malloc(sizeof(GLfloat) * 2 * vertexCount);
	GLuint *indexArray = (GLuint *)malloc(sizeof(GLuint) * triangleCount * 3);

	for (x = 0; x < terrainTexture.width; x++)
	{
		for (z = 0; z < terrainTexture.height; z++)
		{
			vertexArray[(x + z * terrainTexture.width)*3 + 0] = x / terrainScale;
			vertexArray[(x + z * terrainTexture.width)*3 + 1] = terrainTexture.imageData[(x + z * terrainTexture.width) * (terrainTexture.bpp/8)] / 100.0f;
			vertexArray[(x + z * terrainTexture.width)*3 + 2] = z / terrainScale;

			texCoordArray[(x + z * terrainTexture.width)*2 + 0] = (GLfloat)x; // (float)x / tex->width;
			texCoordArray[(x + z * terrainTexture.width)*2 + 1] = (GLfloat)z; // (float)z / tex->height;
		}
	}

	for (x = 0; x < terrainTexture.width; x++)
	{
		for(z = 0; z < terrainTexture.height; z++)
		{

			int i;
			vec3 normal;

			for (i = 0; i < NEIGHBOR; i++)
			{

				//Neighbor directions
				int tx1 = x + direction[i][0];
				int tz1 = z + direction[i][1];
				int tx2 = x + direction[i][2];
				int tz2 = z + direction[i][3];

				bool outside = false;
			
				vec3 v0, v1, v2, tempNormal;

				v0.x = vertexArray[(x + z * terrainWidth) * 3 + 0];
				v0.y = vertexArray[(x + z * terrainWidth) * 3 + 1];
				v0.z = vertexArray[(x + z * terrainWidth) * 3 + 2];

				if (IsOutsideMap(tx1, tz1))
				{
					outside = true;
				} else 
				{
					v1.x = vertexArray[(tx1 + tz1 * terrainWidth) * 3 + 0];
					v1.y = vertexArray[(tx1 + tz1 * terrainWidth) * 3 + 1];
					v1.z = vertexArray[(tx1 + tz1 * terrainWidth) * 3 + 2];
				}

				if (IsOutsideMap(tx2, tz2))
				{
					outside = true;
				} else 
				{
					v2.x = vertexArray[(tx2 + tz2 * terrainWidth) * 3 + 0];
					v2.y = vertexArray[(tx2 + tz2 * terrainWidth) * 3 + 1];
					v2.z = vertexArray[(tx2 + tz2 * terrainWidth) * 3 + 2];
				}

				if (outside)
				{
					tempNormal = SetVector(0,1,0);
				} else 
				{
					tempNormal = CalcNormalVector(v0, v1, v2);
				}

				normal = VectorAdd(normal, tempNormal);
			}

			normal = Normalize(normal);

			normalArray[(x + z * terrainWidth) * 3 + 0] = normal.x;
			normalArray[(x + z * terrainWidth) * 3 + 1] = normal.y;
			normalArray[(x + z * terrainWidth) * 3 + 2] = normal.z;
		} 
	} 

	for (x = 0; x < terrainTexture.width-1; x++)
	{
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
int IsOutsideMap(int x, int z)
{
	return (x < 0 || x >= terrainWidth || z < 0 || z >= terrainHeight) ? 1 : 0;
}



