#version 150

uniform mat4 model;
uniform vec3 cameraPosition;

uniform sampler2D texUnit;
uniform sampler2D texUnit2;

//LIGHT
uniform vec3 lightSourcesDirPosArr[4];
uniform vec3 lightSourcesColorArr[4];
uniform float specularExponent[4];
uniform bool isDirectional[4];
uniform int light_color;

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragVert;

out vec4 finalColor;

void main()
{
	int color = light_color;
	vec3 lightPosition = lightSourcesDirPosArr[color];
	vec3 lightColor = lightSourcesColorArr[color];
	
	vec3 normal = normalize(transpose(inverse(mat3(model))) * fragNormal);
    vec3 surfacePos = vec3(model * vec4(fragVert, 1));
    vec4 surfaceColor = texture(texUnit, fragTexCoord);//*sin(surfacePos.x) + texture(texUnit2, fragTexCoord)*cos(surfacePos.x);
    //texture(texUnit2, fragTexCoord);
	vec3 surfaceToLight;
	if(!isDirectional[color])
	{
		surfaceToLight = normalize(lightPosition - surfacePos);
	}
	else
	{
		surfaceToLight = lightPosition;
	}
    vec3 surfaceToCamera = normalize(cameraPosition - surfacePos);
    
    //ambient
    vec3 ambient = 0.2 * surfaceColor.rgb * vec3(1,1,1); // * surfaceColor;

    //diffuse
    float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));
    vec3 diffuse = diffuseCoefficient * surfaceColor.rgb * lightColor;
    
    //specular
    float specularCoefficient = 0.0;
    if(diffuseCoefficient > 0.0)
        specularCoefficient = pow(max(0.0, dot(surfaceToCamera, reflect(-surfaceToLight, normal))), specularExponent[light_color]);
    vec3 specular = specularCoefficient * vec3(1,1,1) * lightColor;
    
    
    //linear color (color before gamma correction)
    vec3 linearColor = ambient + diffuse + specular;
    
    //final color (after gamma correction)
    vec3 gamma = vec3(1.0/2.2);
    finalColor = vec4(pow(linearColor, gamma), surfaceColor.a);
}