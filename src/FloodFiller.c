#include "FloodFiller.h"
#include <stdio.h>
#include <limits.h>

void FillInit(Fill* fill)
{
	fill->x = 0;
	fill->y = 0;
	fill->minY = 0;
	fill->maxY = 0;
	fill->minX = 0;
	fill->maxX = 0;
}

void FloodFiller(int *map, int size, int x, int y, int target, int original, Fill* fill)
{ 

  int fillL, fillR, i;
  int up = 0;
  int down = 0;
  int in_line = 1;  

  /* find left side, filling along the way */  
  fillL = fillR = x;  

  while( in_line )  
  {  
	map[y + size * fillL] = target;  
    fillL--;  
    in_line = (fillL < 0) ? 0 : (map[y + size * fillL] == original);  
  }		
  fillL++;  

  /* find right side, filling along the way */  
  in_line = 1;  
  while( in_line )  
  {  
    map[y + size * fillR] = target;  
    fillR++;  
    in_line = (fillR > size-1) ? 0 : (map[y + size * fillR] == original);  
  }  
  fillR--;  

  if (fill->minX > fillL)
	fill->minX = fillL;

  if (fill->maxX < fillR)
	fill->maxX = fillR;

  /* search top and bottom */  
  for(i = fillL; i <= fillR; i++)  
  {  
    if( y > 0 && map[y - 1 + size * i] == original )
	{	
		if (up = 0)
		{
			up = 1;

			if (fill->minY > y - 1)
				fill->minY -= 1;
		}

		FloodFiller(map, size, i, y - 1, target, original, fill, 1);  
	}
    
    if( y < size-1 && map[y + 1 + size * i] == original ) 
	{
		if (down == 0)
		{
			down = 1;

			if (fill->maxX < y + 1)
				fill->maxY += 1;
		}

		FloodFiller(map, size, i, y + 1, target, original, fill, 1);  
	}
  } 
}