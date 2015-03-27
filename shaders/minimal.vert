#version 150

in  vec3 in_Position;
in 	vec3 in_Normal;
in vec2 in_TexCoord;


out vec2 tex_Coord;
out float t;
out vec3 normal;
out vec3 position;



// NY

uniform float time;
uniform mat4 projection;
uniform mat4 modelView;



void main(void)
{
	mat3 modelView3 = mat3(modelView);
	normal =  normalize(modelView3 * in_Normal);
	position = modelView3 * in_Position;
	 
	
	
	gl_Position = projection * modelView * vec4(in_Position, 1.0);
	t = time;
	tex_Coord = in_TexCoord;
}
