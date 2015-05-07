#include "windows.h"
#include "GL/glew.h"
#include "WorldObject.h"
#include <stdlib.h>
#include "stdio.h"


WorldObject* WorldObject_New(Model* model, GLuint program, GLuint texture, mat4 * worldView, mat4 * projectionMatrix, float posx, float posy, float posz, float rotx, float roty, float rotz)
{
	WorldObject * object = (WorldObject*)malloc(sizeof(WorldObject));
		
	object->model = model;
	
	object->program = program;
	object->texture = texture;
	object->posx = posx;
	object->posy = posy;
	object->posz = posz;

	object->rotx = rotx;
	object->roty = roty;
	object->rotz = rotz;

	object->worldView = worldView;
	object->projectionMatrix = projectionMatrix;

	return object;
}
void Draw_WorldObject(WorldObject * obj, vec3 sun)
{
	mat4 total = IdentityMatrix();
	obj->translation = T(obj->posx, obj->posy, obj->posz); 
	obj->rotation = Mult(Rz(obj->rotz),  Mult(Rx(obj->rotx) , Ry(obj->roty) ));
	obj->modelMatrix = Mult(obj->translation, obj->rotation);

	

	glUseProgram(obj->program);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	total = Mult( *(obj->worldView), obj->modelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(obj->program, "mdlMatrix"), 1, GL_TRUE, total.m);
	glUniform3fv(glGetUniformLocation(obj->program, "solarPosition"), 1, &(sun.x));

	// Bind Our Texture grass
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, obj->texture);		
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	DrawModel(obj->model, obj->program, "inPosition", "inNormal", "inTangent", "inBitangent", "inTexCoord", "inColor");

}