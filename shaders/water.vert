#version 150

in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexCoord;

out vec3 position;
out vec3 normal;
out vec2 texCoord;

uniform mat4 projMatrix;
uniform mat4 modelToWorld;
uniform mat4 worldToView;

uniform vec3 d1, d2, d3;
uniform float l1, l2, l3;
uniform float a1, a2, a3;
uniform float s1, s2, s3;

uniform float time;

float freq(float wavelength)
{
	return sqrt(9.8 * 2.0 * 3.14159265358979323846 / wavelength);
}

float phase(float speed, float wavelength)
{
	return speed * 2.0 * 3.14159265358979323846 / wavelength;
}

float steepness(float frequency, float amplitude)
{
	return 1.0 / (frequency * amplitude * 3);
}

void main(void)
{	

	const float PI = 3.14159265358979323846;

	//Wave dispersions
	vec3 W = vec3(freq(l1), freq(l2), freq(l3));

	//Wave phase
	vec3 PHI = vec3(phase(s1, l1), phase(s2, l2), phase(s3, l3));

	//Wave amplitude
	vec3 A = vec3(a1, a2, a3);

	//Wave directions
	mat3 D = mat3(d1, d2, d3);

	//Wave steepness
	vec3 Q = vec3(steepness(W[0], A[0]), steepness(W[1], A[1]), steepness(W[2], A[2]));

	vec3 P = vec3(inPosition.x, inPosition.y, inPosition.z);
	for (int i = 0; i < 3; i++)
	{
		P.x += Q[i] * A[i] * D[i].x * cos(W[i] * dot(D[i], P) + PHI[i] * time);
		P.z += Q[i] * A[i] * D[i].z * cos(W[i] * dot(D[i], P) + PHI[i] * time);
		P.y += A[i] * sin(W[i] * dot(D[i], P) + PHI[i] * time);
	}

	//WA for counting normals
	vec3 WA = vec3(W[0] * A[0], W[1] * A[1], W[2] * A[2]);

	vec3 B = vec3(0,0,0);
	vec3 T = vec3(0,0,0);
	vec3 N = vec3(0,0,0);

	for (int i = 0; i < 3; i++)
	{
		float WA = W[i] * A[i];
		float S = sin(W[i] * dot(D[i], P) + PHI[i] * time);
		float C = cos(W[i] * dot(D[i], P) + PHI[i] * time);

		B.x += Q[i] * pow(D[i].x, 2) * WA * S;
		B.z += Q[i] * D[i].x * D[i].z * WA * S;
		B.y += D[i].x * WA * C;

		T.x += Q[i] * D[i].x * D[i].z * WA * S;
		T.z += Q[i] * pow(D[i].z, 2) * WA * S;
		T.y += D[i].z * WA * C;

		N.x += D[i].x * WA * C;
		N.z += D[i].z * WA * C;
		N.y += Q[i] * WA * S;
 	}

	B.x = 1.0 - B.x;
	B.z = -1.0 * B.z; 

	T.x = -1.0 * T.x;
	T.z = 1.0 - T.z;

	N.x = -1.0 * N.x;
	N.z = -1.0 * N.z;
	N.y = 1.0 - N.y;
	
	normal = vec3(N.x, -N.y, N.z);
	position = P;
	texCoord = inTexCoord;

	gl_Position = projMatrix * worldToView * modelToWorld * vec4(P, 1);
}
