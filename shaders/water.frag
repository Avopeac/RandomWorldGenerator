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
	mat3 NM = MWIT;
	
	vec3 lightPosition = normalize(solarPosition);
	vec3 surfacePosition = normalize(MW3 * position);

	vec3 surfaceNormal = normalize(NM * normal);
	vec3 surfaceBinormal = normalize(NM * binormal);
	vec3 surfaceTangent = normalize(NM * tangent);
	mat3 TBN = mat3(surfaceTangent, surfaceBinormal, surfaceNormal);

	vec3 bumpNormal = texture(tex, texCoord / 10.0).xyz;
	bumpNormal = 2.0 * bumpNormal - vec3(1,1,1);
	bumpNormal = TBN * bumpNormal;

	vec3 surfaceVector = normalize(lightPosition - surfacePosition);
	vec3 reflectVector = normalize(2 * bumpNormal * dot(surfaceVector, bumpNormal) - surfaceVector);
	vec3 cameraVector = normalize(-surfacePosition);

	vec3 reflNormal = bumpNormal * vec3(0.03, 0.03, 1.0);

	vec4 screenPos = normalize(worldToView * modelToWorld * vec4(position, 1));
	vec2 reflProjUV = vec2(0.5, 0.5) - vec2(screenPos.x, -screenPos.y);
	vec4 reflection = texture(refl, reflProjUV + reflNormal.xy); 

	float ndotl = max(dot(surfaceVector, reflNormal), 0);
	float facing = 1.0 - ndotl;
	float fresnelFac = fresnel(facing, 0.2, 2.0);

	vec4 waterColor = vec4(0, 0.1, 0.115, 0.7);
	waterColor = waterColor * facing;
	reflection = fresnelFac * reflection;

	float diffuseAngle = max(0.0, dot(reflNormal, cameraVector));
	float kd = 0.5 * diffuseAngle;
	vec4 diffuseColor = kd * vec4(0.6, 0.7, 0.8, 1.0);

	float specularAngle = max(0.0, dot(reflectVector, cameraVector));
	float ks = 0.4 * specularAngle;
	vec4 specularColor = ks * vec4(1, 1, 1, 1) * pow(specularAngle, 32);

	outColor = waterColor + reflection + specularColor + diffuseColor;
}
