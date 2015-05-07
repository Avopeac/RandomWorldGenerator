#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "loadobj.h"
#include "LoadTGA.h"
#include "HeightMapTerrain.h"
#include "Tilemap.h"
#include "Resources.h"
#include "Color.h"

//To get the 1-ring neighborhood of a point
#define NEIGHBOR 6
#define NEIGHBOR_DIR 4
int direction[NEIGHBOR][NEIGHBOR_DIR] = 
{
	{1,0,0,1}, {0,1,-1,1}, {-1,1,-1,0},
	{-1,0,0,-1}, {0,-1,1,-1}, {1,-1,1,0},
};

//Tilemap
Tilemap* tilemap;
//The terrain model
Model* terrainModel;

//The height map texture data
TextureData terrainTexture;

//The scale to sample with
float terrainScale = STANDARD_MAP_SCALE;

float *dt;
mat4* proj;
mat4* mw;
mat4* wv;

GLuint terrainProgram;
GLuint grass;
GLuint sand;
GLuint rock;

GLuint grass_normal;
GLuint sand_normal;
GLuint rock_normal;

void SetupHeightMapTerrain(GLfloat *deltaTime, mat4 *modelWorld, mat4 *worldView, mat4 *projectionMatrix)
{
	dt = deltaTime;
	mw = modelWorld;
	wv = worldView;
	proj = projectionMatrix;

	terrainProgram = loadShaders(TERRAIN_VERT_SHADER, TERRAIN_FRAG_SHADER);


	glUseProgram(terrainProgram);
	printError("init shader");
	
	//Upload to GPU
	glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "projMatrix"), 1, GL_TRUE, proj->m);
	glUniform1i(glGetUniformLocation(terrainProgram, "grass"), 0); // Texture unit 0
	glUniform1i(glGetUniformLocation(terrainProgram, "sand"), 1); // Texture unit 1
	glUniform1i(glGetUniformLocation(terrainProgram, "rock"), 2); // Texture unit 2

	glUniform1i(glGetUniformLocation(terrainProgram, "grass_normal"), 3); // Texture unit 0
	glUniform1i(glGetUniformLocation(terrainProgram, "sand_normal"), 4); // Texture unit 1
	glUniform1i(glGetUniformLocation(terrainProgram, "rock_normal"), 5); // Texture unit 2

	LoadTGATextureSimple(GRASS_1_TEXTURE, &grass);
	LoadTGATextureSimple(SAND_1_TEXTURE, &sand);
	LoadTGATextureSimple(ROCK_1_TEXTURE, &rock);
	
	LoadTGATextureSimple(GRASS_1_NORMAL, &grass_normal);
	LoadTGATextureSimple(SAND_1_NORMAL, &sand_normal);
	LoadTGATextureSimple(ROCK_1_NORMAL, &rock_normal);
}

void DrawHeightMapTerrain(vec3 sun)
{
	mat4 total;

	glUseProgram(terrainProgram);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	total = Mult(*wv, *mw);
	glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "mdlMatrix"), 1, GL_TRUE, total.m);
	glUniform3fv(glGetUniformLocation(terrainProgram, "solarPosition"), 1, &(sun.x));
	
	// Bind Our Texture grass
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grass);		
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	// Bind Our Texture sand
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sand);		
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	// Bind Our Texture grass
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, rock);		
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	
	// Bind Our Texture grass normal
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, grass_normal);		
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	// Bind Our Texture sand normal
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, sand_normal);		
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	// Bind Our Texture grass
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, rock_normal);		
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	DrawModel(GetTerrainModel(), terrainProgram, "inPosition", "inNormal", "inTangent", "inBitangent", "inTexCoord", "inColor");
	DrawWorldObjects(tilemap);

	
}

//Generates the terrain model based on scale and the set height map texture data
void GenerateTerrain()
{
	unsigned int vertexCount = terrainTexture.width * terrainTexture.height;
	unsigned int triangleCount = (terrainTexture.width-1) * (terrainTexture.height-1) * 2;
	unsigned int x, z;

	Color* tempColor = Color_New(0,0,0);
	const Color* colorGrass = Color_New(1,0,0);
	const Color* colorSand = Color_New(0,1,0);
	const Color* colorRock = Color_New(0,0,1);

	
	int xplus,xminus,yplus,yminus;
	int colorCount;

	GLfloat *vertexArray = (GLfloat *)malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *normalArray = (GLfloat *)malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *tangentArray = (GLfloat *)malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *bitangentArray = (GLfloat *)malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *texCoordArray = (GLfloat *)malloc(sizeof(GLfloat) * 2 * vertexCount);
	GLfloat *colorArray = (GLfloat *)malloc(sizeof(GLfloat)*3 * vertexCount);
	GLuint *indexArray = (GLuint *)malloc(sizeof(GLuint) * triangleCount * 3);
	
	tilemap = Tilemap_New(terrainTexture.width,wv,proj);
	generateTileMap(tilemap);
	
	
	
	
	for (x = 0; x < terrainTexture.width; x++)
	{
		for (z = 0; z < terrainTexture.height; z++)
		{
			vertexArray[(x + z * terrainTexture.width)*3 + 0] = x / terrainScale;
			vertexArray[(x + z * terrainTexture.width)*3 + 1] = -getHeightWrapping(tilemap, x,z);
			vertexArray[(x + z * terrainTexture.width)*3 + 2] = z / terrainScale;

			texCoordArray[(x + z * terrainTexture.width)*2 + 0] = (GLfloat)x; // (float)x / tex->width;
			texCoordArray[(x + z * terrainTexture.width)*2 + 1] = (GLfloat)z; // (float)z / tex->height;

			
			xplus = x;
			xminus = x-1;
			yplus = z;
			yminus = z-1;
				
			tempColor = getColor(getColorWrapping(tilemap, xplus, yplus));
			colorCount = 1;
			if(xminus >= 0 && yminus >= 0) 
			{
				tempColor = addColor(tempColor, getColor(getColorWrapping(tilemap, xminus, yminus)));
				colorCount++;
			}
			if(xminus >= 0)
			{
				tempColor = addColor(tempColor, getColor(getColorWrapping(tilemap, xminus, yplus)));
				colorCount++;
			}
			if(yminus >= 0)
			{				
				tempColor = addColor(tempColor, getColor(getColorWrapping(tilemap, xplus, yminus)));
				colorCount++;
			}
			tempColor = divColor(tempColor, colorCount);
			colorArray[(x + z * terrainTexture.width)*3 + 0] = tempColor->r;
			colorArray[(x + z * terrainTexture.width)*3 + 1] = tempColor->g;
			colorArray[(x + z * terrainTexture.width)*3 + 2] = tempColor->b;

		}
	}
	
	for (x = 0; x < terrainTexture.width; x++)
	{
		for(z = 0; z < terrainTexture.height; z++)
		{

			int i;
			vec3 normal, tangent, bitangent;
			vec3 c1, c2;
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
		
			c1 = CrossProduct(normal, SetVector(0,0,1));
			c2 = CrossProduct(normal, SetVector(0,1,0));

			if(Norm(c1) > Norm(c2))
			{
				tangent = c1;
			}
			else
			{
				tangent = c2;
			}
			tangent = Normalize(tangent);
			bitangent = CrossProduct(normal, tangent);
			bitangent = Normalize(bitangent);

			tangentArray[(x + z * terrainWidth) * 3 + 0] = tangent.x;
			tangentArray[(x + z * terrainWidth) * 3 + 1] = tangent.y;
			tangentArray[(x + z * terrainWidth) * 3 + 2] = tangent.z;
			
			bitangentArray[(x + z * terrainWidth) * 3 + 0] = bitangent.x;
			bitangentArray[(x + z * terrainWidth) * 3 + 1] = bitangent.y;
			bitangentArray[(x + z * terrainWidth) * 3 + 2] = bitangent.z;

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
		tangentArray,
		bitangentArray,
		texCoordArray,
		colorArray,
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

//Getter for tilemap (call only after generation)
Tilemap* getTilemap()
{
	return tilemap;
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



