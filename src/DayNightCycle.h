#include "VectorUtils3.h"
#include "GL_utilities.h"

//The ambient color 
vec3 ambientColorDay;
vec3 ambientColorNight; 

//Update the day time with delta time
void UpdateDayTime(float deltaTime);

//
void SetTime(int hours, int minutes, int seconds);
