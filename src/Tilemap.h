#define GRASS 0
#define SAND 1
#define ROCK 2

typedef struct Tilemap
{
	double* heights;
	int* colors;
	int size;
}Tilemap;

Tilemap* Tilemap_New(int size);
void setHeight(Tilemap* tilemap, int x, int y, double height);
void setHeightWrapping(Tilemap* tilemap, int x, int y, double height);
double getHeight(Tilemap* tilemap, int x, int y);
double getHeightWrapping(Tilemap* tilemap, int x, int y);
void setColorWrapping(Tilemap * tilemap, int x, int y, int color);
int getColorWrapping(Tilemap * tilemap, int x, int y);

void generateTileMap( Tilemap*  tilemap);
void divide(Tilemap * tilemap, int map_size, int div_size, int stop, int minHeight, int maxHeight);
void diamond(Tilemap* tilemap, int map_size,int x, int y, int size, double offset, int minHeight, int maxHeight);
void square(Tilemap * tilemap, int map_size,int x, int y, int size, double offset, int minHeight, int maxHeight);
double average(double* values, int n);
float random_value();
void setTilemapColors(Tilemap * tilemap);
double clamp(double value, int min, int max);
void smoothMap(Tilemap* tilemap, int scale, int radius);
double calculateAvgHeight(Tilemap* tilemap, int x, int y, int r);
