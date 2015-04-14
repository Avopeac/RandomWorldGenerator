#version 150

out vec4 outColor;

in vec2 texCoord;
in vec3 normal;
in vec3 position;

uniform sampler2D tex;
uniform mat4 modelToWorld;
uniform mat4 worldToView;
uniform vec3 solarPosition;

void main(void)
{
	vec3 lightPosition = normalize(mat3(modelToWorld) * solarPosition);
	vec4 lightColor = vec4(1, 1, 1, 1);

	vec3 surfacePosition = normalize(mat3(modelToWorld) * position);
	vec4 surfaceColor = 0.5 * texture(tex, texCoord);

	vec3 surfaceNormal = normalize(transpose(inverse(mat3(modelToWorld))) * normal);
	float lightSurfAngle = max(0.0, dot(surfaceNormal, normalize(lightPosition -  surfacePosition)));

	outColor = surfaceColor + clamp(lightColor * lightSurfAngle, 0, 0.5);
}
