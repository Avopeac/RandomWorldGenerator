#version 150

out vec4 finalColor;
in vec2 texCoord;
in vec3 fragNormal;
in vec3 fragTangent;
in vec3 fragBitangent;
in vec3 fragVert;
in vec3 fragColor;

uniform sampler2D grass;
uniform sampler2D sand;
uniform sampler2D rock;


uniform sampler2D grass_normal;
uniform sampler2D sand_normal;
uniform sampler2D rock_normal;

uniform mat4 mdlMatrix;
uniform mat4 modelToWorld;
uniform mat4 worldToView;
uniform vec3 solarPosition;

void main(void)
{
	vec3 lightPosition = normalize(mat3(mdlMatrix) * solarPosition);
	vec3 lightColor = vec3(1,0.8,0.8);
	
	//Normalize vectors
	vec3 normal = normalize(fragNormal);
	vec3 tangent = normalize(fragTangent);
	vec3 bitangent = normalize(fragBitangent);
	vec3 surfacePos = normalize(fragVert);
	
	//Add all textures together
	vec4 surfaceColor = (texture(grass, texCoord))*fragColor.r;
	surfaceColor += (texture(sand, texCoord))*fragColor.g;
	surfaceColor += (texture(rock, texCoord))*fragColor.b;
	
	//calculate bump normal
	vec3 bumpNormal = (texture2D( grass_normal, texCoord ).xyz) * fragColor.r;
	bumpNormal += (texture2D(sand_normal, texCoord).xyz) * fragColor.g;
	bumpNormal += (texture2D(rock_normal, texCoord).xyz) * fragColor.b;
	bumpNormal = 2.0 * bumpNormal - vec3(1,1,1);
	
	mat3 TBN = mat3(tangent, bitangent, normal);
	normal = TBN * bumpNormal;
	normal = normalize(normal);
	
	//setting this to zero, getting very view dependent lighting
	surfacePos = vec3(0.0);
    vec3 surfaceToCamera = normalize(-surfacePos);
	vec3 surfaceToLight = normalize(lightPosition - surfacePos);
	//surfaceToLight = mat3(mdlMatrix) * surfaceToLight; 
	
	//ambient
    vec3 ambient = 0.05 * surfaceColor.rgb * vec3(1,1,1); // * surfaceColor;

    //diffuse
    float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));
    vec3 diffuse = diffuseCoefficient * surfaceColor.rgb * lightColor;
    
    //specular
    float specularCoefficient = 0.0;
    if(diffuseCoefficient > 0.0)
	{
		vec3 r = 2*normal*dot(surfaceToLight, normal) - surfaceToLight;
		specularCoefficient = pow(max(0.0, dot(surfaceToCamera, r)), 10);
	}
	
    vec3 specular = vec3(0,0,0); //specularCoefficient * vec3(1,1,1) * lightColor;
    
    
    //linear color (color before gamma correction)
    vec3 linearColor = clamp(ambient + diffuse + specular, 0, 1);
    
    //final color (after gamma correction)
    vec3 gamma = vec3(1.0/2.2);
    finalColor = vec4(pow(linearColor, gamma), surfaceColor.a);
}
