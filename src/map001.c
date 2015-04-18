#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_hitbox.h>

#include <GFraMe/GFraMe_object.h>

#include <stdlib.h>
#include <string.h>

/** Generated tilemap */
char map001_tilemap[] = 
{
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,64,65,65,65,65,66,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,96,97,97,97,97,98,131,131,131,131,131,70,71,72,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,64,65,65,65,99,97,97,97,97,98,131,131,131,131,131,102,103,104,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,96,97,97,97,97,97,97,97,97,98,131,131,131,131,131,134,135,136,131,131,131,131,131,131,
  65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,99,97,97,97,97,97,97,97,97,100,65,65,65,65,65,65,65,65,65,65,65,65,65,65,
  97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,
  97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,
};

/** Generated tilemap's width */
int map001_width = 40;
/** Generated tilemap's height */
int map001_height = 30;

/** Get all this map's walls into a GFraMe_object buffer */
int map001_getWalls(GFraMe_object **ppObjs, int *pLen, int *pUsed) {
    int len;
    int i;
    
    len = 4;
    
    if (!ppObjs)
        return 1;
    if (!pLen)
        return 1;
    if (!pUsed)
        return 1;
    
    if (*pLen < len) {
        *ppObjs = (GFraMe_object*)realloc(*ppObjs, sizeof(GFraMe_object)*len);
        if (!(*ppObjs))
            return 1;
        *pLen = len;
    }
    *pUsed = len;
    
    i = 0;
    while (i < *pLen)
        memset(&((*ppObjs)[i++]), 0, sizeof(GFraMe_object));
    
    i = 0;
    GFraMe_object_clear(&((*ppObjs)[i]));
    GFraMe_object_set_x(&((*ppObjs)[i]), 0);
    GFraMe_object_set_y(&((*ppObjs)[i]), 216);
    GFraMe_hitbox_set(&((*ppObjs)[i].hitbox), GFraMe_hitbox_upper_left, 0/*x*/, 0/*y*/, 320, 24);
    
    i++;
    GFraMe_object_clear(&((*ppObjs)[i]));
    GFraMe_object_set_x(&((*ppObjs)[i]), 128);
    GFraMe_object_set_y(&((*ppObjs)[i]), 200);
    GFraMe_hitbox_set(&((*ppObjs)[i].hitbox), GFraMe_hitbox_upper_left, 0/*x*/, 0/*y*/, 32, 16);
    
    i++;
    GFraMe_object_clear(&((*ppObjs)[i]));
    GFraMe_object_set_x(&((*ppObjs)[i]), 160);
    GFraMe_object_set_y(&((*ppObjs)[i]), 184);
    GFraMe_hitbox_set(&((*ppObjs)[i].hitbox), GFraMe_hitbox_upper_left, 0/*x*/, 0/*y*/, 48, 32);
    
    i++;
    GFraMe_object_clear(&((*ppObjs)[i]));
    GFraMe_object_set_x(&((*ppObjs)[i]), 248);
    GFraMe_object_set_y(&((*ppObjs)[i]), 192);
    GFraMe_hitbox_set(&((*ppObjs)[i].hitbox), GFraMe_hitbox_upper_left, 0/*x*/, 0/*y*/, 24, 8);
    
    i++;
    return 0;
}