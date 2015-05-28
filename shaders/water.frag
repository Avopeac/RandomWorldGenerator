#version 150

out vec4 outColor;

in vec2 texCoord;
in vec3 normal;
in vec3 binormal;
in vec3 tangent;
in vec3 position;

in float depth;

uniform sampler2D refl;
uniform sampler2D tex;
uniform mat4 modelToWorld;
uniform mat4 worldToView;
uniform mat4 projectionMatrix;
uniform vec3 solarPosition;
uniform float solarAltitude;

float fresnel(float da, float bias, float power)
{
	return max(bias + (1.0 - bias) * pow(da, power), 0.0);
}

void main(void)
{
	const float PI = 3.14159265358979323846;

	mat3 MWIT = transpose(inverse(mat3(modelToWorld)));
	mat3 MW3 = mat3(modelToWorld);
	mat3 WV3 = mat3(worldToView);
	
	vec3 lightPosition = normalize(solarPosition);
	vec3 surfacePosition = normalize(MW3 * position);

	vec3 surfaceNormal = -1.0 * normalize(MWIT * normal);
	vec3 surfaceBinormal = -1.0 * normalize(MWIT * binormal);
	vec3 surfaceTangent = -1.0 * normalize(MWIT * tangent);
	mat3 TBN = mat3(surfaceTangent, surfaceBinormal, surfaceNormal);

	vec3 bumpNormalA = texture(tex, texCoord / 100).xyz;
	vec3 bumpNormalB = texture(tex, texCoord / 24).xyz;
	vec3 bumpNormalC = texture(tex, texCoord / 12).xyz;
	vec3 bumpNormalD = texture(tex, texCoord / 2).xyz;

	vec3 bumpNormal = 2.0 * ((bumpNormalA + bumpNormalB + bumpNormalC + bumpNormalD) * 0.25) - vec3(1,1,1);
	bumpNormal = normalize(TBN * bumpNormal);

	vec3 surfaceVector = normalize(lightPosition - surfacePosition);
	vec3 reflectVector = normalize(2 * bumpNormal * dot(surfaceVector, bumpNormal) - surfaceVector);
	vec3 cameraVector = normalize(-surfacePosition);

	vec3 reflNormal = bumpNormal * vec3(0.05, 0.05, 0.05);

	vec4 screenPos = normalize(worldToView * modelToWorld * vec4(position, 1));
	vec2 reflProjUV = vec2(0.5, 0.5) - vec2(screenPos.x, -screenPos.y);
	vec4 reflection = texture(refl, reflProjUV + reflNormal.xy); 

	float ndotl = max(dot(surfaceVector, bumpNormal), 0);
	float facing = 1.0 - ndotl;
	float fresnelFac = fresnel(facing, 0.9, 2.0);

	vec4 waterColor = vec4(0, 0.14, 0.115, 1.0);
	waterColor = waterColor * facing;
	reflection = fresnelFac * reflection;

	float diffuseAngle = max(0.0, dot(bumpNormal, cameraVector));
	float kd = diffuseAngle;
	vec4 diffuseColor = kd * vec4(0.0, 0.7, 0.8, 1.0);

	float specularAngle = max(0.0, dot(reflectVector, cameraVector));
	float ks = specularAngle;
	vec4 specularColor = ks * vec4(1, 1, 1, 1) * pow(specularAngle, 12);

	outColor = waterColor + reflection + specularColor + diffuseColor;
}
