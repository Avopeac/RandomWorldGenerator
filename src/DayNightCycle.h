#include "VectorUtils3.h"
#include "GL_utilities.h"

#define STD_SECONDS_PER_MINUTE 60
#define STD_MINUTES_PER_HOUR 60
#define STD_HOURS_PER_DAY 24

#define GREENWICH_NOON 2451545.0f

#define LONGITUDE_STHLM_SWEDEN 18.0f
#define LATITUDE_STHLM_SWEDEN 59.0f

typedef struct DistributionCoefficient
{
	float a, b, c, d, e;
} DistributionCoefficient;

typedef struct SolarPosition
{
	float declinationAngle;
	float azimuthAngle;
	float zenithAngle;
	float julianDay;
	float solarTime;
	float localDecimalHours;
	float latitude;
	float longitude;
	float standardMeridian;
} SolarPosition; 

void SetupDayNightCycle(GLfloat *deltaTime, mat4 *modelWorld, mat4 *worldView, mat4 *projectionMatrix);
void InitDayNightCycle(int year, int month, int day, int sec, float daySpeed, float latitude, float longitude, float GMT);
void DrawDayNightCycle();

void UpdateSolarPosition(float hours);
void UpdateDayTime(float deltaTime, float daySpeed);
void SetTimeMeasures(int hours, int minutes, int seconds);
void SetDate(int year, int month, int day);


int CalcJulianDay(int year, int month, int day);
float CalcSolarTime(int julian, float hours, float latitude, float meridian);
float CalcSolarDeclination(int julian);
float CalcStandardMeridian(float GMT);
float CalcSolarZenithAngle(float latitude, float declination, float solarTime);
float CalcSolarAzimuthAngle(float latitude, float declination, float solarTime);
void CalcAbsoluteZenithValues(float turbidity, float zenithAngle);

void PrintSolarPosition();