#version 150

out vec4 finalColor;
in vec2 texCoord;
in vec3 fragNormal;
in vec3 fragVert;

uniform sampler2D tex;
uniform mat4 mdlMatrix;

void main(void)
{
	vec3 lightPosition = vec3(100, -50, 0);
	vec3 lightColor = vec3(1,0.8,0.8);

	vec3 normal = normalize(fragNormal);
	vec3 surfacePos = fragVert;
	vec4 surfaceColor = (texture(tex, texCoord));
	
    vec3 surfaceToCamera = normalize(-surfacePos);
	vec3 surfaceToLight = normalize(lightPosition - surfacePos);
	surfaceToLight = mat3(mdlMatrix) * surfaceToLight; 
	
	
	 //ambient
    vec3 ambient = 0.2 * surfaceColor.rgb * vec3(1,1,1); // * surfaceColor;

    //diffuse
    float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));
    vec3 diffuse = diffuseCoefficient * surfaceColor.rgb * lightColor;
    
    //specular
    float specularCoefficient = 0.0;
    if(diffuseCoefficient > 0.0)
	{
		//reflect(surfaceToLight, normal)
		vec3 r = 2*normal*dot(surfaceToLight, normal) - surfaceToLight;
		specularCoefficient = pow(max(0.0, dot(surfaceToCamera, r)), 10);
   
	}
	
         vec3 specular = vec3(0,0,0);//specularCoefficient * vec3(1,1,1) * lightColor;
    
    
    //linear color (color before gamma correction)
    vec3 linearColor = clamp(ambient + diffuse + specular, 0, 1);
    
    //final color (after gamma correction)
    vec3 gamma = vec3(1.0/2.2);
    finalColor = vec4(pow(linearColor, gamma), surfaceColor.a);
}
