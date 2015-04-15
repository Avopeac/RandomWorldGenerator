#version 150

out vec4 out_Color;

in vec3 normal;
in vec3 position;

uniform float azimuth;
uniform float zenith;
uniform float turbidity;
uniform float luminanceZ;
uniform float zenithX;
uniform float zenithY;

//Calculate luminance distribution
float perez(float z, float g, float a, float b, float c, float d, float e)
{
	return (1 + a * exp(b / cos(z))) * (1 + c * exp(d * g) + e * pow(cos(g), 2));
}

//Angle between the sun and the observer direction 
float gamma(float zenith, float azimuth, float observerZenith, float observerAzimuth)
{
	return acos(sin(zenith) * sin(observerZenith) * cos(observerAzimuth - azimuth) + cos(zenith) * cos(observerZenith));
}


//Converting to CIE xyY color space
vec4 colorize(float Y, float x, float y)
{
	float X = x / y * Y; 
	float Z = (1 - x - y) / y * Y;

	vec4 fragColor;

	fragColor.a = 1;
	fragColor.r = 3.2406 * X - 1.5372 * Y - 0.4986 * Z;
	fragColor.g = -0.9689 * X + 1.8758 * Y + 0.0415 * Z;
	fragColor.b = 0.0557 * X - 0.2040 * Y + 1.0570 * Z;

	return fragColor;
}

void main(void)
{	

	const float PI = 3.14159265358979323846;

	//Calculate pixel's horizontal coordinate system
	float observerAzimuth = atan(position.z, position.x);
	float observerZenith = acos(position.y / length(position));

	//This is a bit wasteful but I'm not sending all these variables as uniforms.
	//Distribution coefficients:
	//a = darkening or brightening of the horizon
	//b = luminance gradient near the horizon
	//c = relative intensity of the circumsolar region
	//d = width of the circumsolar region
	//e = relative backscattered light

	float Ya = 0.1787 * turbidity - 1.4630;
	float Yb = -0.3554 * turbidity + 0.4275;
	float Yc = -0.0227 * turbidity + 5.3251;
	float Yd = 0.1206 * turbidity - 2.5771;
	float Ye = -0.0670 * turbidity + 0.3703;

	float xa = -0.0193 * turbidity - 0.2592;
	float xb = -0.0665 * turbidity + 0.0008;
	float xc = -0.0004 * turbidity + 0.2125;
	float xd = -0.0641 * turbidity - 0.8989;
	float xe = -0.0033 * turbidity + 0.0452;

	float ya = -0.0167 * turbidity - 0.2608;
	float yb = -0.0950 * turbidity + 0.0092;
	float yc = -0.0079 * turbidity + 0.2102;
	float yd = -0.0441 * turbidity - 1.6537;
	float ye = -0.0109 * turbidity + 0.0529;

	float g = gamma(zenith, azimuth, observerZenith, observerAzimuth);
	
	float zMin = min(observerZenith, PI / 2.0);

	float Yp = luminanceZ * perez(zMin, g, Ya, Yb, Yc, Yd, Ye) / perez(0, zenith, Ya, Yb, Yc, Yd, Ye);
	float xp = zenithX * perez(zMin, g, xa, xb, xc, xd, xe) / perez(0, zenith, xa, xb, xc, xd, xe);
	float yp = zenithY * perez(zMin, g, ya, yb, yc, yd, ye) / perez(0, zenith, ya, yb, yc, yd, ye);

	out_Color = colorize(Yp, xp, yp);
}