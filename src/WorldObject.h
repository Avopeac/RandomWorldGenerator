#include "loadobj.h"
#include "VectorUtils3.h"

#ifndef WORLDOBJECT
#define WORLDOBJECT
typedef struct WorldObject
{
	Model * model;
	GLuint program;
	GLuint texture;
	float posx, posy, posz;
	float rotx, roty, rotz;
    mat4 rotation;
    mat4 translation;
    mat4 modelMatrix;
   
	mat4 modelView;
	mat4 * worldView;
	mat4 * projectionMatrix;

}WorldObject;
#endif
WorldObject* WorldObject_New(Model* model, GLuint program, GLuint texture, mat4 * worldView, mat4 * projectionMatrix, 
	float posx, float posy, float posz, float rotx, float roty, float rotz);
void Draw_WorldObject(WorldObject * obj);