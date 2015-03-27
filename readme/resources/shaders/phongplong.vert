#version 150

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

in vec3 in_Position;
in vec2 in_TexCoord;
in vec3 in_Normal;

out vec3 fragVert;
out vec2 fragTexCoord;
out vec3 fragNormal;

void main()
{
	fragTexCoord = in_TexCoord;
	fragNormal = in_Normal;
	fragVert = in_Position;
	
	gl_Position = projection * view * model * vec4(in_Position, 1);
}
