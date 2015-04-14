#version 150

in vec3 inPosition;
in vec3 inNormal;

out vec2 texCoord;
out vec3 normal;
out vec3 position;

uniform mat4 projMatrix;
uniform mat4 modelToWorld;
uniform mat4 worldToView;

void main(void)
{
	position = vec3(modelToWorld * vec4(inPosition, 1));
	gl_Position = projMatrix * mat4(mat3(worldToView)) * modelToWorld * vec4(inPosition,1.0);
}
