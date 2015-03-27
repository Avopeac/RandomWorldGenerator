#version 150

out vec4 out_Color;

in vec2 tex_Coord;
in float t;
in vec3 normal;
in vec3 position;


const vec3 light = vec3(0.58, 0.58, 0.58);
const vec4 diff = vec4(0.8, 0.8, 0.8, 0.8);

uniform sampler2D texUnit;

//LIGHT
uniform vec3 lightSourcesDirPosArr[4];
uniform vec3 lightSourcesColorArr[4];
uniform float specularExponent[4];
uniform bool isDirectional[4];

void main(void)
{
	vec3 lightDir = lightSourcesDirPosArr[1];
	vec3 lightColor = lightSourcesColorArr[2];
	float specularity = specularExponent[1];
	
	vec4 ambientLight = vec4(0.2, 0.2, 0.2, 1);
	vec4 final_color = ambientLight + vec3(texture2D(texUnit, tex_Coord));
	
	vec3 N = normalize(normal);
	vec3 L = normalize(lightDir);
	
	float lambert = dot(N,L);
	if(lambert > 0.0)
	{
		
		final_color += lightColor * lambert;
		
		vec3 E = normalize(position);
		vec3 R = reflect(-L, N);
		float specular = pow(max(dot(R,E), 0.0), 25);
		final_color += specular * specularity;
	}
	gl_FragColor = final_color;
	//
	//vec4 ambientLight = vec4(0.2, 0.2, 0.2, 1);
	//gl_FragColor = clamp(ambientLight + vec4(diffuse
    //    * vec3(texture2D(texUnit, tex_Coord))), 0.0, 1.0);
		
	//gl_FragColor = texture2D(texUnit, vec2(position.x, position.z));
}
