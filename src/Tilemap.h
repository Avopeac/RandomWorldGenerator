typedef struct Tilemap
{
	int* heights;
	int size;
}Tilemap;

Tilemap* Tilemap_New(int size);
void setHeight(Tilemap* tilemap, int x, int y, int height);
int getHeight(Tilemap* tilemap, int x, int y);
void generateTileMap( Tilemap*  tilemap);
void divide(Tilemap * tilemap, int map_size, int div_size, int stop, int minHeight, int maxHeight);
void diamond(Tilemap* tilemap, int map_size,int x, int y, int size, int offset, int minHeight, int maxHeight);
void square(Tilemap * tilemap, int map_size,int x, int y, int size, int offset, int minHeight, int maxHeight);
int average(int* values, int n);
int clamp(int value, int min, int max);
void smoothMap(Tilemap* tilemap, int scale, int radius);
int calculateAvgHeight(Tilemap* tilemap, int x, int y, int r);
