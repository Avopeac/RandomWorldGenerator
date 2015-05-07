#include "windows.h"
#include "Tilemap.h"
#include <time.h>
#include <stdlib.h>
#include "stdio.h"
#include "LoadTGA.h"
#include "loadobj.h"
#include "GL_utilities.h"
#include "Resources.h"

Tilemap* Tilemap_New(int size, mat4* worldView, mat4* projectionMatrix)
{
	
	double * heights = (double*)malloc(sizeof(double)*size*size);
	int * colors = (int*)malloc(sizeof(int)*size*size);
	int * set = (int*)malloc(sizeof(int)*size*size);
	int x,y, index;
	Tilemap* tilemap;
	for(x = 0; x < size; x++)
	{
		for(y = 0; y < size; y++)
		{
			index = x + y*size;
			heights[index] = 0;
			colors[index] = 0;
			set[index] = 0;
		}
	}

	tilemap = (Tilemap*)malloc(sizeof(Tilemap));
	tilemap->heights = heights;
	tilemap->colors = colors;
	tilemap->objectSet = set;
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
	return ret; 	
}

void setColorWrapping(Tilemap * tilemap, int x, int y, int color)
{
	int xw = (x + tilemap->size)%tilemap->size;
	int yw = (y + tilemap->size)%tilemap->size;
	tilemap->colors[ xw +  yw*tilemap->size  ] = color;

}

int getColorWrapping(Tilemap * tilemap, int x, int y)
{
	int xw = (x + tilemap->size)%tilemap->size;
	int yw = (y + tilemap->size)%tilemap->size;
	return tilemap->colors[ xw +  yw*tilemap->size  ];
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
		setTilemapColors(tilemap);
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

void setTilemapColors(Tilemap* tilemap)
{
	int x,y;
	double height;
	int color;
	for(x = 0; x < tilemap->size; x++)
	{
		for(y = 0; y < tilemap->size; y++)
		{
			height = getHeightWrapping(tilemap,x,y);
			if(height > 6.5f)
			{
				setColorWrapping(tilemap, x,y, SAND);
			}
			else if(height > 3 )
			{
				setColorWrapping(tilemap, x,y,GRASS);
			}
			else
			{
				setColorWrapping(tilemap, x, y, ROCK);
			}
		}
	}
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

/*
void GenerateWorldObjects(Tilemap * tilemap)
{
	int x,y;
	float random;
	Model * model;
	GLuint grass,sand,rock, grass_normal, sand_normal, rock_normal;
	GLuint program;
	program = loadShaders(OBJECT_VERT_SHADER, OBJECT_FRAG_SHADER);

	glUseProgram(program);
	
	//Upload to GPU
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, tilemap->projectionMatrix->m);
	glUniform1i(glGetUniformLocation(program, "grass"), 0); // Texture unit 0
	glUniform1i(glGetUniformLocation(program, "sand"), 1); // Texture unit 1
	glUniform1i(glGetUniformLocation(program, "rock"), 2); // Texture unit 2

	glUniform1i(glGetUniformLocation(program, "grass_normal"), 3); // Texture unit 3
	glUniform1i(glGetUniformLocation(program, "sand_normal"), 4); // Texture unit 4
	glUniform1i(glGetUniformLocation(program, "rock_normal"), 5); // Texture unit 5

	LoadTGATextureSimple(GRASS_1_TEXTURE, &grass);
	LoadTGATextureSimple(SAND_1_TEXTURE, &sand);
	LoadTGATextureSimple(ROCK_1_TEXTURE, &rock);
	
	LoadTGATextureSimple(GRASS_1_NORMAL, &grass_normal);
	LoadTGATextureSimple(SAND_1_NORMAL, &sand_normal);
	LoadTGATextureSimple(ROCK_1_NORMAL, &rock_normal);

	
	model = LoadModelPlus("models/bunnyplus.obj");
	srand(time(NULL));
	for(x = 0; x < tilemap->size; x++)
	{
		for(y = 0; y < tilemap->size; y++)
		{
			random  = random_value();
			if(random > 0.98)
			{
				//Hardcoded map scale
				WorldObject * obj = WorldObject_New(model, program, grass, tilemap->worldView, tilemap->projectionMatrix,
					x/4,-getHeightWrapping(tilemap, x,y),y/4,0,0,0);
				//fprintf(stderr, "Object Added: %d %d :: %.3f", x,y ,random);
				
				setObjectWrapping(tilemap, x, y, obj);
			}
		}
	}
	
}

void DrawWorldObjects(Tilemap * tilemap)
{
	int x,y;
	WorldObject * obj;
	for(x = 0; x < tilemap->size; x++)
	{
		for(y = 0; y < tilemap->size; y++)
		{
			obj = getObjectWrapping(tilemap, x, y);
			if(getObjectSetWrapping(tilemap,x,y))
				Draw_WorldObject(obj);
		}
	}
}
	
*/