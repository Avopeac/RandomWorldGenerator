#include <stdlib.h>

typedef struct Fill 
{
	int x, y;
	int maxX, maxY;
	int minX, minY;
}Fill;

void FloodFiller(int *map, int size, int x, int y, int target, int original, Fill* fill, int recursion);

void FillInit(Fill* fill);
