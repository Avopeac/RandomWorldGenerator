#include "windows.h"
#include "Color.h"
#include "Tilemap.h"

Color* Color_New(float r, float g, float b)
{
	Color * color = (Color*)malloc(sizeof(Color)); 
	color->r = r;
	color->g = g;
	color->b = b;
	return color;
}

void setColor(Color * color, float r, float g, float b)
{
	color->r = r;
	color->g = g;
	color->b = b;
}

Color * getColor(int color)
{
	switch(color)
	{
		case GRASS:
			  return Color_New(1,0,0);
		case SAND: 
			return Color_New(0,1,0);
		case ROCK:
			return Color_New(0,0,1);
		default: 
			return Color_New(1,0,0);
	}
}

Color * addColor(Color * color1, Color * color2)
{
	return Color_New(color1->r + color2->r, color1->g + color2->g, color1->b + color2->b);
}

Color * subColor(Color * color1, Color * color2)
{
	return Color_New(color1->r - color2->r, color1->g - color2->g, color1->b - color2->b);
}

Color * divColor(Color * color, float div)
{
	if(div == 0) return color;
	return Color_New(color->r/div, color->g/div, color->b/div);
}

Color * multColor(Color * color, float mult)
{
	return Color_New(color->r*mult, color->g*mult, color->b*mult);
}