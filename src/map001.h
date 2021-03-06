#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_object.h>

#include "sprite.h"

/** Generated tilemap */
extern char map001_tilemap[];
/** Generated tilemap's width */
extern int map001_width;
/** Generated tilemap's height */
extern int map001_height;
/** Get all this map's walls into a GFraMe_object buffer */
int map001_getWalls(GFraMe_object **ppObjs, int *pLen, int *pUsed);
/** Get all this map's walls into a GFraMe_object buffer */
int map001_getStones(sprite ***pppSprs, int *pLen, int *pUsed);
/** Get all this map's walls into a GFraMe_object buffer */
int map001_getSpikes(sprite ***pppSprs, int *pLen, int *pUsed);
