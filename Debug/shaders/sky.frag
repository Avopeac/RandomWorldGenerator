#version 150

out vec4 out_Color;

in vec2 tex_Coord;
in float t;
in vec3 normal;
in vec3 position;


uniform sampler2D texUnit;

void main(void)
{
	
	gl_FragColor = texture2D(texUnit, tex_Coord);
}
