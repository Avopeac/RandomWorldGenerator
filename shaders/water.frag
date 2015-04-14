#version 150

out vec4 outColor;

in vec2 texCoord;
in vec3 normal;
in vec3 binormal;
in vec3 tangent;
in vec3 position;

uniform sampler2D tex;
uniform mat4 modelToWorld;
uniform mat4 worldToView;
uniform vec3 solarPosition;
uniform float solarAltitude;

void main(void)
{

	const float PI = 3.14159265358979323846;

	vec3 lightPosition = mat3(worldToView) * mat3(modelToWorld) * solarPosition;
	vec3 surfacePosition = vec3(mat4(worldToView) * mat4(modelToWorld) * vec4(position, 1));

	float alt = clamp(PI / 2 - solarAltitude, PI / 18, PI / 2);
	vec4 lightColor = vec4(0.6, 0.8, 1.0, 1 * alt);

	mat3 normalMatrix = mat3(worldToView) * transpose(inverse(mat3(modelToWorld)));

	vec3 surfaceNormal = normalize(normalMatrix * normal);
	//vec3 surfaceBinormal = normalize(normalMatrix * binormal);
	//vec3 surfaceTangent = normalize(normalMatrix * tangent);

	vec3 surfaceVector = normalize(lightPosition - surfacePosition);
	vec3 reflectVector = normalize(2 * surfaceNormal * dot(surfaceVector, surfaceNormal) - surfaceVector);
	vec3 cameraVector = normalize(-surfacePosition);

	float diffuseAngle = max(0.0, dot(surfaceNormal, surfaceVector));
	float specularAngle = max(0.0, dot(reflectVector, cameraVector));

	float ks = 2.0 * specularAngle;
	float kd = 0.2 * diffuseAngle;;

	float transparency = 1.0 - 1.0 / exp(-surfacePosition.z * 0.3);

	vec4 ambientColor = kd * texture(tex, texCoord);
	vec4 diffuseColor = kd * lightColor * diffuseAngle;
	vec4 specularColor = ks * lightColor * pow(specularAngle, 50);

	outColor = vec4(0.6 * alt, 0.8 * alt, 1.0 * alt, transparency) + ambientColor + diffuseColor + specularColor;
}
