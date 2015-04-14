#include "WaterSource.h"
#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "Resources.h"
#include "LoadTGA.h"
#include "FBOHelper.h"
#include <stdlib.h>
#include <time.h>

GLuint waterProgram;

float *dt;
float t;

mat4 *mw;
mat4 *wv;
mat4 *proj;
vec3 *solarPosition;
GLuint waterTex;


float GetRandom()
{
	return (float)(256.0f - (rand() % 512)) / 256.0f;
}

void SetupWaterSources(float *deltaTime, mat4 *modelToWorld, mat4 *worldToView, mat4 *projectionMatrix)
{

	dt = deltaTime;
	mw = modelToWorld;
	wv = worldToView;
	proj = projectionMatrix;

	LoadTGATextureSimple(WATER_512_TEXTURE, &waterTex);

	waterProgram = loadShaders(WATER_VERT_SHADER, WATER_FRAG_SHADER);
	glUseProgram(waterProgram);
	
	glUniform1i(glGetUniformLocation(waterProgram, "tex"), 1);
	glUniformMatrix4fv(glGetUniformLocation(waterProgram, "projMatrix"), 1, GL_TRUE, proj->m);

	//Use FBO's for texturing water
	InitFBO();

	srand(time(NULL));
}

void SetSolarPosition(vec3 *position)
{
	solarPosition = position;
}

WaterSource* GenerateWaterSource(vec3 p, unsigned int sx, unsigned int sz, float l1, float l2, float l3, float a1, float a2, float a3, float s1, float s2, float s3, vec3 d1, vec3 d2, vec3 d3)
{
	WaterSource* source;
	Model* tempModel;

	float tessFact = 10.0f;

	unsigned int vertexCount = sx * sz;
	unsigned int triangleCount = (sx - 1) * (sz - 1) * 2;
	unsigned int x, z;

	GLfloat *vertexArray = (GLfloat *)malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *normalArray = (GLfloat *)malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *texCoordArray = (GLfloat *)malloc(sizeof(GLfloat) * 2 * vertexCount);
	GLuint *indexArray = (GLuint *)malloc(sizeof(GLuint) * triangleCount * 3);

	for (x = 0; x < sx; ++x)
		for (z = 0; z < sz; ++z)
		{
			vertexArray[(x + z * sx)*3 + 0] = (GLfloat)x / tessFact;
			vertexArray[(x + z * sx)*3 + 1] = 0;
			vertexArray[(x + z * sx)*3 + 2] = (GLfloat)z / tessFact;

			texCoordArray[(x + z * sx)*2 + 0] = (GLfloat)x / tessFact;
			texCoordArray[(x + z * sx)*2 + 1] = (GLfloat)z / tessFact;

			normalArray[(x + z * sx)*3 + 0] = 0;
			normalArray[(x + z * sx)*3 + 1] = 1;
			normalArray[(x + z * sx)*3 + 2] = 0;
		}

	for (x = 0; x < sx - 1; x++)
	{
		for (z = 0; z < sz - 1; z++)
		{ 
			//Triangle 1
			indexArray[(x + z * (sx - 1))*6 + 0] = x + z * sx;
			indexArray[(x + z * (sx - 1))*6 + 1] = x + (z + 1) * sx;
			indexArray[(x + z * (sx - 1))*6 + 2] = x + 1 + z * sx;

			//Triangle 2
			indexArray[(x + z * (sx - 1))*6 + 3] = x + 1 + z * sx;
			indexArray[(x + z * (sx - 1))*6 + 4] = x + (z + 1) * sx;
			indexArray[(x + z * (sx - 1))*6 + 5] = x + 1 + (z + 1) * sx;
		}
	}

	// Create Model and upload to GPU:
	tempModel = LoadDataToModel(
		vertexArray,
		normalArray,
		texCoordArray,
		NULL,
		indexArray,
		vertexCount,
		triangleCount*3);


	//Create a new watersource with the mesh
	source = (WaterSource*)malloc(sizeof(WaterSource));

	//Set center position
	source->p = SetVector(
		p.x - (sx - sx / 2.0f) / tessFact,
		p.y,
		p.z - (sz - sz / 2.0f) / tessFact
		);

	//Save handles to important data
	source->water = tempModel;
	source->sx = sx;
	source->sz = sz;
	source->time = 0.0f;

	//Wavelengths
	source->l1 = l1;
	source->l2 = l2;
	source->l3 = l3;

	//Amplitudes
	source->a1 = a1;
	source->a2 = a2;
	source->a3 = a3;

	//Speeds
	source->s1 = s1;
	source->s2 = s2;
	source->s3 = s3;

	//Directions
	source->d1 = d1;
	source->d2 = d2;
	source->d3 = d3;

	return source;
}

void DrawWaterSource(WaterSource *source)
{
	mat4 tempModelWorld, tempWorldView;

	//Add time
	source->time += (*dt) / 1000.0f;

	glUseProgram(waterProgram);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, waterTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	tempModelWorld = *mw;
	tempWorldView = *wv;

	tempModelWorld = T(source->p.x, source->p.y, source->p.z);
	glUniformMatrix4fv(glGetUniformLocation(waterProgram, "modelToWorld"), 1, GL_TRUE, tempModelWorld.m);
	glUniformMatrix4fv(glGetUniformLocation(waterProgram, "worldToView"), 1, GL_TRUE, tempWorldView.m);

	glUniform3fv(glGetUniformLocation(waterProgram, "d1"), 1, &(source->d1.x));
	glUniform3fv(glGetUniformLocation(waterProgram, "d2"), 1, &(source->d2.x));
	glUniform3fv(glGetUniformLocation(waterProgram, "d3"), 1, &(source->d3.x));

	glUniform1f(glGetUniformLocation(waterProgram, "l1"), source->l1);
	glUniform1f(glGetUniformLocation(waterProgram, "l2"), source->l2);
	glUniform1f(glGetUniformLocation(waterProgram, "l3"), source->l3);

	glUniform1f(glGetUniformLocation(waterProgram, "a1"), source->a1);
	glUniform1f(glGetUniformLocation(waterProgram, "a2"), source->a2);
	glUniform1f(glGetUniformLocation(waterProgram, "a3"), source->a3);

	glUniform1f(glGetUniformLocation(waterProgram, "s1"), source->s1);
	glUniform1f(glGetUniformLocation(waterProgram, "s2"), source->s2);
	glUniform1f(glGetUniformLocation(waterProgram, "s3"), source->s3);

	glUniform3fv(glGetUniformLocation(waterProgram, "solarPosition"), 1, &(solarPosition->x));
	glUniform1f(glGetUniformLocation(waterProgram, "time"), source->time);
	DrawModel(source->water, waterProgram, "inPosition", "inNormal", "inTexCoord");

}