#include "windows.h"
#include "Tilemap.h"
#include <time.h>
#include <stdlib.h>
#include "stdio.h"


Tilemap* Tilemap_New(int size)
{
	
	double * heights = (double*)malloc(sizeof(double)*size*size);
	int x,y;
	Tilemap* tilemap;
	for(x = 0; x < size; x++)
	{
		for(y = 0; y < size; y++)
		{
			heights[x + y*size] = 0;
		}
	}

	tilemap = (Tilemap*)malloc(sizeof(Tilemap));
	tilemap->heights = heights;
	tilemap->size = size;

	return tilemap;
}



void setHeight(Tilemap* tilemap, int x, int y, double height)
{
	tilemap->heights[x + y*tilemap->size] = height; 
}

void setHeightWrapping(Tilemap* tilemap, int x, int y, double height)
{
	int xw = (x + tilemap->size)%tilemap->size;
	int yw = (y + tilemap->size)%tilemap->size;
	tilemap->heights[ xw +  yw*tilemap->size  ] = height;
	/*if(x < 0 || x > tilemap->size - 1 || y < 0 || y > tilemap->size - 1)
	{
		fprintf(stderr, "Outside map: %d, %d\n", x,y);
		return;
	}
	
	tilemap->heights[x + y*tilemap->size] = height; 
	
	//fprintf(stderr, "Tile: %d,%d. Value: %.3f\n", x,y, height);
	*/
}

double getHeight(Tilemap* tilemap, int x, int y)
{
	return tilemap->heights[x + y * tilemap->size];
}

double getHeightWrapping(Tilemap* tilemap, int x, int y)
{
	int max = tilemap->size - 1;
	int xw = (x + tilemap->size)%tilemap->size;
	int yw = (y + tilemap->size)%tilemap->size;
	double ret = tilemap->heights[ xw +  yw*tilemap->size  ]; 
	/*
	if(x < 0 || x > tilemap->size - 1 || y < 0 || y > tilemap->size - 1)
	{
		fprintf(stderr, "Tile: %d,%d. New Tile: %d,%d\n", x,y,xw,yw);

//		fprintf(stderr, "Outside map: %d, %d\n", x,y);
	}
	*/
	return ret; 
	
	//return tilemap->heights[x + y * tilemap->size];
	
}


void generateTileMap( Tilemap*  tilemap)
{
        /* 0  	1  	5
		 * 
		 * 1   	5  	7
		 *
		 * 5  	7  	10	
		 *
		 */

	
		int mapsize = tilemap->size;
        int end = tilemap->size;
        //TOP ROW
       /* tilemap->heights[0] = 0; //LT
        tilemap->heights[end / 2] = 1; //MT
        tilemap->heights[end] = 5; //RT

        //MID ROW
        tilemap->heights[end / 2 * mapsize] = 1; //LM
        tilemap->heights[end / 2 * mapsize + end / 2] = 5; //MID
        tilemap->heights[end / 2 * mapsize + end] = 7; //RM

        //BOT ROW
        tilemap->heights[end*mapsize] = 5; //LB
        tilemap->heights[end*mapsize + end / 2] = 7; //MB
        tilemap->heights[end * mapsize + end] = 10; //RB

    */
		
        divide(tilemap, end, end/2, 1, -12, 12);
		smoothMap(tilemap, 4,8);
		smoothMap(tilemap, 1,1);

}

void divide(Tilemap * tilemap, int map_size, int div_size, int stop, int minHeight, int maxHeight)
{
	int x,y;
	float roughness = 0.15;
	int half = div_size/2;
	double scale = (double)(roughness*(div_size/2 + 4));
	if(half < stop) return;

	srand(time(NULL));

	for(y = half; y < map_size; y+=div_size)
	{
		for(x = half; x < map_size; x+=div_size)
		{
			square(tilemap, map_size,x, y, half, (double)(random_value() * scale), minHeight, maxHeight);
		}
	}
	for(y = 0; y <= map_size; y+=half)
	{
		for(x = (y + half) % div_size; x <= map_size; x+=div_size)
		{
			diamond(tilemap, map_size, x, y, half, (double)(random_value() * scale), minHeight, maxHeight);
		}
	}
	divide(tilemap, map_size, div_size/2, stop, minHeight, maxHeight);
}

void diamond(Tilemap* tilemap, int map_size,int x, int y, int size, double offset, int minHeight, int maxHeight)
{

	int index = x + (y - size)*map_size;
	double values[] = {
		getHeightWrapping(tilemap, x,y-size),
		getHeightWrapping(tilemap, x + size, y),
		getHeightWrapping(tilemap, x, y + size),
		getHeightWrapping(tilemap, x - size, y)};

	double ave = clamp(average(values, 4) + offset, minHeight, maxHeight);
	setHeightWrapping(tilemap, x, y, ave);
}

void square(Tilemap * tilemap, int map_size,int x, int y, int size, double offset, int minHeight, int maxHeight)
{
	double values[] = {
		getHeightWrapping(tilemap, x - size, y - size),
		getHeightWrapping(tilemap, x + size, y - size),
		getHeightWrapping(tilemap, x + size, y + size),
		getHeightWrapping(tilemap, x - size, y + size)};

	double ave = clamp(average(values, 4) + offset, minHeight, maxHeight);
	setHeightWrapping(tilemap, x, y, ave);
}


double average(double* values, int n)
{
	int i;
	double tempval;
	float total=0;
	int div=0;
	for(i = 0; i < n; i++)
	{
		tempval = values[i];
		if(tempval > 0)
		{
			total+=tempval;
			div++;
		}
	}

	if(div > 0)
	{
		return total/div;
	}
	return 0;
}

double clamp(double value, int min, int max)
{
	if(value < min) return min;
	else if(value > max) return max;
	else return value;
}

float random_value()
{
	int random = rand()%200;
	float ret = (float)( ((float)random)/100 - 1); 
	return ret;
}
void smoothMap(Tilemap* tilemap, int scale, int radius)
{
	int x,y;
	for(x = 0; x < tilemap->size/scale; x++)
	{
		for(y = 0; y < tilemap->size/scale; y++)
		{
			tilemap->heights[x*scale + y*scale*tilemap->size] = calculateAvgHeight(tilemap, x*scale, y*scale, radius);
				
		}
	}
}

double calculateAvgHeight(Tilemap* tilemap, int x, int y, int r)
{
	int i,j;
	double h;
	double total = 0;
	int div = 0;

	for(i = x-r; i < x+r+1; i+=r)
	{
		for(j = y-r; j < y+r+1; j+=r)
		{

			if(i > 0 && i < tilemap->size && j > 0 && j < tilemap->size)
			{
				h = tilemap->heights[i + j*tilemap->size];
				total+= h;
				div++;
			}
					
		}
	}
	if(div <= 0) return 0;
	return (double)(total/(double)div);
}
	
