#include "windows.h"
#include "GL/glew.h"
#include "ObjectManager.h"
#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "Resources.h"
#include "LoadTGA.h"
#include "HeightMapTerrain.h"
#include <time.h>


#define MAX_OBJECTS 2048
//Array of all objects in the world
WorldObject ** objects;
int objectCounter;

float *dt;
mat4* proj;
mat4* mw;
mat4* wv;

GLuint objectProgram;

GLuint tree;
GLuint sand;
GLuint rock;

GLuint grass_normal;
GLuint sand_normal;
GLuint rock_normal;

void SetupObjectManager(float * deltaTime, mat4 *modelWorld, mat4 *worldView, mat4 *projectionMatrix)
{
	dt = deltaTime;
	mw = modelWorld;
	wv = worldView;
	proj = projectionMatrix;

	objectProgram = loadShaders(OBJECT_VERT_SHADER, OBJECT_FRAG_SHADER);

	glUseProgram(objectProgram);

	glUniformMatrix4fv(glGetUniformLocation(objectProgram, "projMatrix"), 1, GL_TRUE, proj->m);
	glUniform1i(glGetUniformLocation(objectProgram, "grass"), 0); // Texture unit 0
	glUniform1i(glGetUniformLocation(objectProgram, "sand"), 1); // Texture unit 1
	glUniform1i(glGetUniformLocation(objectProgram, "rock"), 2); // Texture unit 2

	glUniform1i(glGetUniformLocation(objectProgram, "grass_normal"), 3); // Texture unit 0
	glUniform1i(glGetUniformLocation(objectProgram, "sand_normal"), 4); // Texture unit 1
	glUniform1i(glGetUniformLocation(objectProgram, "rock_normal"), 5); // Texture unit 2

	LoadTGATextureSimple(TREE_TEXTURE, &tree);
	LoadTGATextureSimple(SAND_1_TEXTURE, &sand);
	LoadTGATextureSimple(ROCK_1_TEXTURE, &rock);
	
	LoadTGATextureSimple(GRASS_1_NORMAL, &grass_normal);
	LoadTGATextureSimple(SAND_1_NORMAL, &sand_normal);
	LoadTGATextureSimple(ROCK_1_NORMAL, &rock_normal);

	objects = (WorldObject**)malloc(sizeof(WorldObject*)*MAX_OBJECTS);
	objectCounter = 0;
}

void DrawObjectManager(vec3 sun, CameraData cam)
{
	int i;
	//fprintf(stderr, "\nNumber of objects %d", objectCounter);

	for(i = 0; i < objectCounter; i++)
	{
		Draw_WorldObject(objects[i], sun, cam);
	}
}

void GenerateObjects()
{
	int x,y;
	float random;
	Model * model;
	Tilemap* tilemap;
	WorldObject * obj = (WorldObject*)malloc(sizeof(WorldObject*));


	model = LoadModelPlus("models/tred.obj");
	tilemap = getTilemap();

	//Init RNG
	srand(time(NULL));
	for(x = 0; x < tilemap->size; x++)
	{
		for(y = 0; y < tilemap->size; y++)
		{
			random  = random_value();
			if(random > 0.995)
			{
				//Hardcoded map scale
				 obj = WorldObject_New(model, objectProgram, tree, wv, proj,
					x/4,-getHeightWrapping(tilemap, x,y) - 0.3 ,y/4,0,0,0);
				//fprintf(stderr, "Object Added: %d %d :: %.3f", x,y ,random);
				objects[objectCounter] = obj;	
				objectCounter++;
				if(objectCounter >= MAX_OBJECTS) return;
			}
		}
	}
}