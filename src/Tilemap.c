#include "windows.h"
#include "Tilemap.h"
#include <time.h>
#include <stdlib.h>
#include "stdio.h"


Tilemap* Tilemap_New(int size)
{
	
	int * heights = (int*)malloc(sizeof(int)*size*size);
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



void setHeight(Tilemap* tilemap, int x, int y, int height)
{
	tilemap->heights[x + y*tilemap->size] = height; 
}

int getHeight(Tilemap* tilemap, int x, int y)
{
	return tilemap->heights[x + y * tilemap->size];
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
        int end = tilemap->size - 1;
        //TOP ROW
        tilemap->heights[0] = 0; //LT
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

    
		srand(time(NULL));
        divide(tilemap, end, 8, 1, -120, 120);
		smoothMap(tilemap, 4,8);
		smoothMap(tilemap, 1,1);

}

void divide(Tilemap * tilemap, int map_size, int div_size, int stop, int minHeight, int maxHeight)
{
	int x,y;
	float roughness = 0.15f;

	int half = div_size/2;
	int scale = (int)(roughness*(div_size + 2));
	if(half < stop) return;

	for(y = half; y < map_size; y+=div_size)
	{
		for(x = half; x < map_size; x+=div_size)
		{
			square(tilemap, map_size,x, y, half, (int)(rand()%maxHeight * scale * 2 - scale), minHeight, maxHeight);
		}
	}
	for(y = 0; y <= map_size; y+=half)
	{
		for(x = (y + half) % div_size; x <= map_size; x+=div_size)
		{
			diamond(tilemap, map_size, x, y, half, (int)(rand()%maxHeight * scale * 2 - scale), minHeight, maxHeight);
		}
	}
	divide(tilemap, map_size, div_size/2, stop, minHeight, maxHeight);
}

void diamond(Tilemap* tilemap, int map_size,int x, int y, int size, int offset, int minHeight, int maxHeight)
{
	int index = x + (y - size)*map_size;
	int values[] = {
		tilemap->heights[x + (y-size)*map_size],
		tilemap->heights[(x+size + y*map_size)],
		tilemap->heights[x + (y+size)*map_size],
		tilemap->heights[x-size + y*map_size] };
	int ave = average(values, 4);
	tilemap->heights[x + y*map_size] = clamp(ave + offset, minHeight, maxHeight);
}

void square(Tilemap * tilemap, int map_size,int x, int y, int size, int offset, int minHeight, int maxHeight)
{
	int values[] = {
		tilemap->heights[x-size + (y-size)*map_size],
		tilemap->heights[x+size + (y-size)*map_size],
		tilemap->heights[x+size, (y+size)*map_size],
		tilemap->heights[x-size, (y+size)*map_size]};
	int ave = average(values,4);
	tilemap->heights[x + y * map_size] = clamp(ave + offset, minHeight, maxHeight);
}


int average(int* values, int n)
{
	int i;
	int tempval;
	int total=0;
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

int clamp(int value, int min, int max)
{
	if(value < min) return min;
	else if(value > max) return max;
	else return value;
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

int calculateAvgHeight(Tilemap* tilemap, int x, int y, int r)
{
	int i,j,h;
	int total = 0;
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
	return (int)((float)total/(float)div);
}
	
