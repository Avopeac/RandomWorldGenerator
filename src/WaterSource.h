#include "VectorUtils3.h"
#include "loadobj.h"
#include "GL_utilities.h"

typedef struct WaterSource
{
	Model* water;
	vec3 p;
	unsigned int sx, sz;
	vec3 d1, d2, d3;
	vec3 nd1, nd2, nd3;
	float l1, l2, l3;
	float a1, a2, a3;
	float s1, s2, s3;
	float time;
} WaterSource;

void SetupWaterSources(float *deltaTime, mat4 *modelToWorld, mat4 *worldToView, mat4 *projectionMatrix);

WaterSource* GenerateWaterSource(vec3 p, //position
	unsigned int sx, unsigned int sz, //size
	float l1, float l2, float l3, //wavelength
	float a1, float a2, float a3, //amplitude
	float s1, float s2, float s3, //speed
	vec3 d1, vec3 d2, vec3 d3,
	Model* terrainModel); //directions

void DrawWaterSource(WaterSource *source, vec3 sun, float sunAltitude, vec3 cam);
