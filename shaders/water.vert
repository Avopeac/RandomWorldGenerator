#version 150

in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexCoord;

out vec3 position;
out vec3 normal;
out vec3 binormal;
out vec3 tangent;
out vec2 texCoord;

out float depth;

uniform mat4 projMatrix;
uniform mat4 modelToWorld;
uniform mat4 worldToView;

uniform vec3 d1, d2, d3;
uniform float l1, l2, l3;
uniform float a1, a2, a3;
uniform float s1, s2, s3;

uniform float time;

//Psuedo-random number from 0 to 0.99999...
float rand(vec2 seed){
    return fract(sin(dot(seed.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

//Calculate the wave frequency W for some wavelength and terrain depth.
//Depth is located in the y-coordinate of the plane mesh.
//NOTE: May need some tweaking
float freq(float wavelength, float depth)
{
	float k = 2.0 * 3.14159265358979323846 / wavelength;
	return sqrt(9.8 * k * tanh(k * (1.0 - 1.0 / depth)));
}

//Calculate the phase shift of a wave for some speed and wavelength.
float phase(float speed, float wavelength)
{
	return speed * 2.0 * 3.14159265358979323846 / wavelength;
}

//Calculate the wave curve steepness, high values may cause loops at top.
float steepness(float frequency, float amplitude)
{
	return 1.0 / (frequency * amplitude * 3);
}

//Add some changes to the amplitude based on direction and time.
//A = amplitude, a = scale, f = chance of high wave.
float amplitude(float A, float a, float f)
{
	return A - a * sin(time + inPosition.x) * sin(inPosition.z * f);
}

//Add some random displacement to the starting height of each vertex.
float displacement(float y, float b, float r)
{
	return y * b * sin(r * time);
}

void main(void)
{	

	const float a1s = 0.04; const float a1f = 2.0;
	const float a2s = 0.2; const float a2f = 0.5;
	const float a3s = 0.01; const float a3f = 3.0;

	const float hd = 0.0002;

	//Wave dispersions with respect to depth to terrain (stored in y-coordinate)
	vec3 W = vec3(freq(l1, inPosition.y),
	 freq(l2, inPosition.y),
	 freq(l3, inPosition.y));

	//Wave phase
	vec3 PHI = vec3(phase(s1, l1),
	 phase(s2, l2),
	  phase(s3, l3));

	//Wave amplitude
	vec3 A = vec3(amplitude(a1, a1s, a1f),
	 amplitude(a2, a2s, a2f),
	  amplitude(a3, a3s, a3f));

	//Wave directions
	mat3 D = mat3(d1, d2, d3);

	//Wave steepness
	vec3 Q = vec3(steepness(W[0], A[0]),
	 steepness(W[1], A[1]),
	  steepness(W[2], A[2]));

	//Wave height displacement
	float y = displacement(inPosition.y,
	 hd,
	 rand(inPosition.xz));

	vec3 P = vec3(inPosition.x, y, inPosition.z);
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
	binormal = vec3(B.x, -B.y, B.z);
	tangent = vec3(T.x, -T.y, T.z);

	depth = inPosition.y;
	position = P;
	texCoord = inTexCoord;

	gl_Position = projMatrix * worldToView * modelToWorld * vec4(P, 1);
}
