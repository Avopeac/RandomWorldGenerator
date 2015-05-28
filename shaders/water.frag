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
//	mat3 WV3 = mat3(worldToView);
	
	vec3 lightPosition = normalize(solarPosition);
	vec3 surfacePosition = normalize(MW3 * position);

	vec3 surfaceNormal = normalize(MWIT * normal);
	vec3 surfaceBinormal = normalize(MWIT * binormal);
	vec3 surfaceTangent = normalize(MWIT * tangent);
	mat3 TBN = mat3(surfaceTangent, surfaceBinormal, surfaceNormal);

	vec3 bumpNormalA = texture(tex, texCoord * 0.01).xyz;
	vec3 bumpNormalB = texture(tex, texCoord * 0.04).xyz;
	vec3 bumpNormalC = texture(tex, texCoord * 0.08).xyz;
	vec3 bumpNormalD = texture(tex, texCoord * 0.50).xyz;

	vec3 bumpNormal = 0.5 * (bumpNormalA + bumpNormalB + bumpNormalC + bumpNormalD) - vec3(1);
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
	vec4 diffuseColor = diffuseAngle * vec4(0.0, 0.25, 0.30, 1.0);

	float specularAngle = max(0.0, dot(reflectVector, cameraVector));
	vec4 specularColor = vec4(1) * pow(specularAngle, 12);

	outColor = vec4(vec4(waterColor + reflection + specularColor + diffuseColor).xyz, 0.8);
}
