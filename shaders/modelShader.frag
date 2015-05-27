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
	vec3 lightPosition = normalize(solarPosition);
	vec3 lightColor = vec3(1,0.8,0.8);
	
	//Normalize vectors
	vec3 normal = normalize(fragNormal);
	vec3 tangent = normalize(fragTangent);
	vec3 bitangent = normalize(fragBitangent);
	vec3 surfacePos = normalize(fragVert);
	
	//Add all textures together
	vec4 surfaceColor = (texture(grass, texCoord));
	if (surfaceColor.rgb == vec3(0,0,0))
		surfaceColor.a = 0;
	
    vec3 surfaceToCamera = normalize(-surfacePos);
	vec3 surfaceToLight = normalize(lightPosition);
	
	//ambient
    vec3 ambient = 0.5 * surfaceColor.rgb;

    //diffuse
    float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));
    vec3 diffuse = diffuseCoefficient * surfaceColor.rgb * lightColor;
    
    //specular
    float specularCoefficient = 0.0;
    if(diffuseCoefficient > 0.0)
	{
		vec3 r = 2*normal*dot(surfaceToLight, normal) - surfaceToLight;
		specularCoefficient = pow(max(0.0, dot(surfaceToCamera, r)), 40);
	}
	
    vec3 specular = 0.1 * specularCoefficient * lightColor;
    
    
    //linear color (color before gamma correction)
    vec3 linearColor = clamp(ambient + diffuse + specular, 0, 1);
    
    //final color (after gamma correction)
     vec3 gamma = vec3(1.0/2.2);

	float fallOff = 0.06;
	float fogAmount = 1.0 - exp(fragVert.z * fallOff);
	vec3 fogColor = vec3(0.5, 0.6, 0.7) * max(0.0, lightPosition.y);
	
    finalColor = vec4(mix(linearColor, fogColor, fogAmount), surfaceColor.a);
}
