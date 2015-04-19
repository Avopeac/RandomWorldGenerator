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
	mat3 NM = WV3 * MWIT;
	
	vec3 lightPosition = vec3(worldToView * modelToWorld * vec4(solarPosition, 1));
	vec3 surfacePosition = vec3(worldToView * modelToWorld * vec4(position, 1));

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

	vec3 refrNormal = bumpNormal * vec3(0.075, 0.075, 1.0);
	vec3 reflNormal = bumpNormal * vec3(0.02, 0.02, 1.0);

	vec4 screenPos = normalize(worldToView * modelToWorld * vec4(position, 1));
	vec2 projUV = vec2(0.5, 0.5) - vec2(screenPos.x, -screenPos.y);
	vec4 reflection = texture(refl, projUV + reflNormal.xy); 

	vec4 refractionA = texture(refl, projUV + refrNormal.xy);
	vec4 refractionB = texture(refl, projUV);

	float ndotl = max(dot(cameraVector, reflNormal), 0);
	float facing = 1.0 - ndotl;
	float fresnelFac = fresnel(facing, 0.2, 2.0);

	float dist = clamp(10.0 / depth, 0, 1);
	vec4 waterColor = vec4(0, 0.1, 0.115, 1);
	vec4 deepColor = refractionA * refractionB * dist + (1 - dist) * waterColor;

	waterColor = waterColor * facing + deepColor * (1.0 - facing);

	reflection = fresnelFac * reflection;

	float diffuseAngle = max(0.0, dot(reflNormal, cameraVector));
	float kd = 0.40 * diffuseAngle;
	vec4 diffuseColor = kd * vec4(1);

	float specularAngle = max(0.0, dot(reflectVector, cameraVector));
	float ks = 0.40 * specularAngle;
	vec4 specularColor = ks * vec4(1) * pow(specularAngle, 50);

	vec4 finalColor = waterColor + reflection + specularColor + diffuseColor;

	float fallOff = 0.02;
	float fogAmount = 1.0 - exp(-cameraVector.z * fallOff);
	vec4 fogColor = vec4(0.5, 0.6, 0.7, 1);

	outColor = finalColor; //mix(finalColor, fogColor, fogAmount);
}
