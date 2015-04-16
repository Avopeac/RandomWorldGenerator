typedef struct Color 
{
	float r,g,b;
} Color;

Color* Color_New(float r, float g, float b);
void setColor(Color* color, float r, float g, float b);
Color * getColor(int color);
Color * addColor(Color * color1, Color * color2);
Color * subColor(Color * color1, Color * color2);
Color * divColor(Color * color, float div);
Color * multColor(Color * color, float mult);


