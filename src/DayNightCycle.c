#include "Resources.h"
#include "DayNightCycle.h"
#include "GL_utilities.h"
#include "loadobj.h"
#include "LoadTGA.h"
#include <math.h>

// This model for calculating the sun is taken from a research paper called:
// "A Practical Analytic Model for Daylight" by A.J Preetham, Peter Shirley and Brian Smits from University of Utah, Dept. of Computer Science

//Time settings
float fracSPM = 1.0f / (float)STD_SECONDS_PER_MINUTE;
float fracMPH = 1.0f / (float)STD_MINUTES_PER_HOUR;
float fracHPD = 1.0f / (float)STD_HOURS_PER_DAY;

//Time passed data
float seconds = 0.0f;
float minutes = 0.0f;
float hours = 0.0f;
float days = 0.0f;
float speed = 0.0f;

//Container for solar data
SolarPosition solarPos;

//Turbidity is basically a fraction of light scattering based due to haze (molecules in the air).
//Determines how clear the view is.
float turbidity = 2.5f;
float luminanceZ = 0.0f;
float zenithX = 0.0f;
float zenithY = 0.0f;

//Start time data
int y = 0;
int m = 0;
int d = 0;

GLuint skydomeProgram;
Model *skydome;

GLfloat *dt;
mat4 *wv;
mat4 *mw;
mat4 *proj;

void SetupDayNightCycle(GLfloat *deltaTime, mat4 *modelWorld, mat4 *worldView, mat4 *projectionMatrix)
{
	//Save delta time reference
	dt = deltaTime;

	//Save model view matrix reference
	wv = worldView;
	mw = modelWorld;
	proj = projectionMatrix;

	//Load skydome shaders
	skydomeProgram = loadShaders(SKY_VERT_SHADER, SKY_FRAG_SHADER);

	//Upload to GPU and set to tex unit 1 (could be better)
	glUseProgram(skydomeProgram);
	glUniformMatrix4fv(glGetUniformLocation(skydomeProgram, "projMatrix"), 1, GL_TRUE, proj->m);

	//Load model
	skydome = LoadModelPlus(SKYDOME_MODEL);
}

void InitDayNightCycle(int year, int month, int day, int sec, float daySpeed, float latitude, float longitude, float GMT)
{
	SetDate(year, month, day);
	seconds = (float) sec;
	speed = daySpeed;

	fprintf(stderr, "hello? %f\n", speed);

	UpdateDayTime(*dt, speed);

	solarPos.latitude = latitude;
	solarPos.longitude = longitude;
	solarPos.standardMeridian = CalcStandardMeridian(GMT);
	solarPos.julianDay = (float) CalcJulianDay(year, month, day);
	solarPos.declinationAngle = CalcSolarDeclination((int)solarPos.julianDay);

	//Using this call instead, less code
	UpdateSolarPosition(hours);
}

void DrawDayNightCycle()
{
	glUseProgram(skydomeProgram);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUniform1f(glGetUniformLocation(skydomeProgram, "azimuth"), solarPos.azimuthAngle);
	glUniform1f(glGetUniformLocation(skydomeProgram, "zenith"), solarPos.zenithAngle);
	glUniform1f(glGetUniformLocation(skydomeProgram, "turbidity"), turbidity);
	glUniform1f(glGetUniformLocation(skydomeProgram, "luminanceZ"), luminanceZ);
	glUniform1f(glGetUniformLocation(skydomeProgram, "zenithX"), zenithX);
	glUniform1f(glGetUniformLocation(skydomeProgram, "zenithY"), zenithY);
	
	
	glUniformMatrix4fv(glGetUniformLocation(skydomeProgram, "modelToWorld"), 1, GL_TRUE, mw->m);
	glUniformMatrix4fv(glGetUniformLocation(skydomeProgram, "worldToView"), 1, GL_TRUE, wv->m);
	DrawModel(skydome, skydomeProgram, "inPosition", "inNormal", "inTexCoord");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	UpdateDayTime(*dt, speed);
	UpdateSolarPosition(hours);

	CalcAbsoluteZenithValues(turbidity, solarPos.zenithAngle);
}

int CalcJulianDay(int year, int month, int day)
{
	//Only if later than start of Gregorian calendar
	int a = (int)(365.25 * (year + 4716));
	int b = (int)(30.6001 * (month + 1));

	return (int)(a + b + day - 1524.5);
}

float CalcSolarTime(int julian, float hours, float longitude, float meridian)
{
	//Using the formula from the paper's appendix.
	float a = (float)(0.170f * sin(4 * M_PI * (julian - 80) / 373));
	float b = (float)(-0.129f * sin(2 * M_PI * (julian - 8) / 355));
	float c = (float)(12 * (meridian - longitude) / M_PI);

	return hours + a + b + c;
}

float CalcSolarDeclination(int julian)
{
	//Due to the axial tilt of earth the sun rays have different angles to the equator depending on day.
	return (float)(0.4093f * sin(2 * M_PI * (julian - 81) / 368));
}

float CalcSolarZenithAngle(float latitude, float declination, float solarTime)
{
	//Using the formula from the paper's appendix.
	float a = (float)(sin(latitude) * sin(declination));
	float b = (float)(cos(latitude) * cos(declination));
	float c = (float)(cos(M_PI * solarTime / 12));

	return (float)(M_PI / 2 - asin(a - b * c));
}

float CalcSolarAzimuthAngle(float latitude, float declination, float solarTime)
{
	//Using the formula from the paper's appendix.
	float a = (float)(-1 * cos(declination));
	float b = (float)(sin(M_PI * solarTime / 12));
	float c = (float)(cos(latitude) * sin(declination));
	float d = (float)(sin(latitude) * cos(declination));
	float e = (float)(cos(M_PI * solarTime / 12));

	//Something wrong with the angles here because sun sets where it dawns.
	return (float)(2.0 * atan(a * b / (c - d * e)));

	return solarTime;
}

float CalcStandardMeridian(float GMT)
{
	//Multiplying hours UTC/GMT with 15 degrees gives us the local time standard meridian in degrees.
	return (float)(15 * M_PI / 180 * GMT);
}

void CalcAbsoluteZenithValues(float turbidity, float zenithAngle)
{
	//Calculating the zenith's luminance, also calculating the maximum possible.
	float x = (float)(4.0f / 9.0f - turbidity / 120.0f);
	float a0 = (float)(M_PI - 2.0f * zenithAngle);
	float a1 = (float)(M_PI);
	float a2 = (float)(4.0453 * turbidity - 4.9710);
	float a3 = (float)(-0.2155 * turbidity + 2.4192);

	//Yz and Y0 where Y0 is the highest possible luminance
	float yz = (float)(a2 * tan(x * a0) + a3);
	float y0 = (float)(a2 * tan(x * a1) + a3);

	//Calculating powers to use later
	GLfloat z = (GLfloat)(solarPos.zenithAngle);
	GLfloat z2 = (GLfloat)(pow(solarPos.zenithAngle, 2));
	GLfloat z3 = (GLfloat)(z2 * solarPos.zenithAngle);

	vec3 turbidities;
	vec3 zx;
	vec3 zy;

	//Normalize the zenith luminance
	luminanceZ = yz / y0;

	//Calculating powers to use later
	turbidities = SetVector((GLfloat)pow(turbidity, 2), turbidity, 1);

	//Calculating chromaticity based on turbidities and angle
	zx = SetVector(0.00166f * z3 - 0.00375f * z2 + 0.00209f * z,
		-0.02903f * z3 + 0.06377f * z2 - 0.03202f * z + 0.00394f,
		0.11693f * z3 - 0.21196f * z2 + 0.06052f * z + 0.25886f);

	zenithX = DotProduct(turbidities, zx);

	zy = SetVector(0.00275f * z3 - 0.00610f * z2 + 0.00317f * z,
		-0.04214f * z3 + 0.08970f * z2 - 0.04153f * z + 0.00516f,
		0.15346f * z3 - 0.26756f * z2 + 0.06670f * z + 0.26688f);

	zenithY = DotProduct(turbidities, zy);
}

void UpdateDayTime(float deltaTime, float daySpeed)
{
	//Add the fractions to each corresponding time unit, also account for speed.
	seconds += deltaTime * 0.001f * daySpeed;
	minutes = seconds * fracSPM;
	hours = minutes * fracMPH;
	days = hours * fracHPD;

	//Add a new julian day.
	if (hours >= STD_HOURS_PER_DAY)
	{
		//Has no check for end of month!
		d++;

		//Reset timer.
		seconds = 0.0f;
		minutes = 0.0f;
		hours = 0.0f;

		//After new julian day we need some recalculations.
		solarPos.julianDay += 1;
		solarPos.declinationAngle = CalcSolarDeclination((int)solarPos.julianDay);
		UpdateSolarPosition(hours);
	}
}

void UpdateSolarPosition(float hours)
{
	//Just counted hours in decimal form.
	solarPos.localDecimalHours = hours;

	//Solar time is an estimate of the passage of time based on the suns position.
	solarPos.solarTime = CalcSolarTime((int)solarPos.julianDay,
		solarPos.localDecimalHours,
		solarPos.longitude,
		solarPos.standardMeridian);

	//The angle of the position of the sun mapped on the equator plane.
	solarPos.azimuthAngle = CalcSolarAzimuthAngle(solarPos.latitude,
		solarPos.declinationAngle,
		solarPos.solarTime);

	//The angle of the position of the sun from its highest point.
	solarPos.zenithAngle = CalcSolarZenithAngle(solarPos.latitude,
		solarPos.declinationAngle,
		solarPos.solarTime);

	//For debug.
	PrintSolarPosition();
}

void SetDate(int year, int month, int day)
{
	//Setting the starting date.
	y = year; m = month; d = day;
}

void PrintSolarPosition()
{
	//Converting most radians to degrees, easier to read.
	fprintf(stderr, "Latitude: %f\n", solarPos.latitude * 180.0f / M_PI);
	fprintf(stderr, "Longitude: %f\n", solarPos.longitude * 180.0f / M_PI);
	fprintf(stderr, "Meridian: %f\n", solarPos.standardMeridian * 180.0f / M_PI);
	fprintf(stderr, "Decimal Hours: %f\n", solarPos.localDecimalHours);
	fprintf(stderr, "Solar Time: %f\n", solarPos.solarTime);
	fprintf(stderr, "Julian Day: %f\n", solarPos.julianDay);
	fprintf(stderr, "Declination Angle: %f\n", solarPos.declinationAngle * 180.0f / M_PI);
	fprintf(stderr, "Zenith Angle: %f\n", solarPos.zenithAngle * 180.0f / M_PI);
	fprintf(stderr, "Azimuth Angle: %f\n", solarPos.azimuthAngle * 180.0f / M_PI);
}