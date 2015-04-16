#version 150

in  vec3 inPosition;
in  vec3 inNormal;
in vec2 inTexCoord;
in vec3 inColor;

out vec2 texCoord;
out vec3 fragNormal;
out vec3 fragVert;
out vec3 fragColor;

// NY
uniform mat4 projMatrix;
uniform mat4 mdlMatrix;

void main(void)
{
	mat3 normalMatrix1 = mat3(mdlMatrix);
	fragNormal = normalize(normalMatrix1 * inNormal);
	fragVert = vec3(mdlMatrix * vec4(inPosition,1));
	texCoord = inTexCoord;
	fragColor = inColor;
	gl_Position = projMatrix * mdlMatrix * vec4(inPosition, 1.0);
}
