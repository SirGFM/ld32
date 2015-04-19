#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_hitbox.h>
#include <GFraMe/GFraMe_object.h>

#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "sprite.h"

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
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,73,74,75,73,74,75,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,105,106,107,105,106,107,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,73,74,75,131,64,65,65,65,65,66,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
  131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,105,106,107,131,96,97,97,97,97,98,131,131,131,131,131,70,71,72,131,131,131,131,131,131,
  131,131,131,131,73,74,75,73,74,75,73,74,75,73,74,75,64,65,65,65,99,97,97,97,97,98,131,131,131,131,131,102,103,104,131,131,131,131,131,131,
  131,131,131,131,105,106,107,105,106,107,105,106,107,105,106,107,96,97,97,97,97,97,97,97,97,98,131,131,131,131,131,134,135,136,131,131,131,131,131,131,
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
    
    len = 6;
    
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
    GFraMe_hitbox_set(&((*ppObjs)[i].hitbox), GFraMe_hitbox_upper_left, 0/*x*/, 0/*y*/, 248, 24);
    
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
    GFraMe_object_clear(&((*ppObjs)[i]));
    GFraMe_object_set_x(&((*ppObjs)[i]), 272);
    GFraMe_object_set_y(&((*ppObjs)[i]), 216);
    GFraMe_hitbox_set(&((*ppObjs)[i].hitbox), GFraMe_hitbox_upper_left, 0/*x*/, 0/*y*/, 48, 24);
    
    i++;
    GFraMe_object_clear(&((*ppObjs)[i]));
    GFraMe_object_set_x(&((*ppObjs)[i]), 248);
    GFraMe_object_set_y(&((*ppObjs)[i]), 224);
    GFraMe_hitbox_set(&((*ppObjs)[i].hitbox), GFraMe_hitbox_upper_left, 0/*x*/, 0/*y*/, 24, 16);
    
    i++;
    return 0;
}
/** Get all this map's walls into a GFraMe_object buffer */
int map001_getStones(sprite ***pppSprs, int *pLen, int *pUsed) {
    int i, len, rv;
    sprite *pSpr;
    
    len = 7;
    
    if (!pppSprs)
        return 1;
    if (!pLen)
        return 1;
    if (!pUsed)
        return 1;
    
    if (*pLen < len) {
        *pppSprs = (sprite**)realloc(*pppSprs, sizeof(sprite*)*len);
        if (!(*pppSprs))
            return 1;
        *pLen = len;
    }
    *pUsed = len;
    
    memset(*pppSprs, 0, sizeof(sprite*)*len);
    
    i = 0;
    pSpr = 0;
    rv = spr_getNew(&pSpr);
    ASSERT_NR(rv == 0);
    (*pppSprs)[i] = pSpr;
    rv = spr_init(pSpr,
             /*x*/40,
             /*y*/194,
          /*offX*/0,
          /*offY*/0,
         /*width*/8,
        /*height*/8,
   /*hitboxWidth*/8,
  /*hitboxHeight*/24,
      /*animData*/_sprRedStoneData,
       /*animLen*/_sprRedStoneAnimLen,
          /*type*/SPR_RED_STONE
    );
    ASSERT_NR(rv == 0);
    i++;
    pSpr = 0;
    rv = spr_getNew(&pSpr);
    ASSERT_NR(rv == 0);
    (*pppSprs)[i] = pSpr;
    rv = spr_init(pSpr,
             /*x*/64,
             /*y*/194,
          /*offX*/0,
          /*offY*/0,
         /*width*/8,
        /*height*/8,
   /*hitboxWidth*/8,
  /*hitboxHeight*/24,
      /*animData*/_sprOrangeStoneData,
       /*animLen*/_sprOrangeStoneAnimLen,
          /*type*/SPR_ORANGE_STONE
    );
    ASSERT_NR(rv == 0);
    i++;
    pSpr = 0;
    rv = spr_getNew(&pSpr);
    ASSERT_NR(rv == 0);
    (*pppSprs)[i] = pSpr;
    rv = spr_init(pSpr,
             /*x*/88,
             /*y*/194,
          /*offX*/0,
          /*offY*/0,
         /*width*/8,
        /*height*/8,
   /*hitboxWidth*/8,
  /*hitboxHeight*/24,
      /*animData*/_sprYellowStoneData,
       /*animLen*/_sprYellowStoneAnimLen,
          /*type*/SPR_YELLOW_STONE
    );
    ASSERT_NR(rv == 0);
    i++;
    pSpr = 0;
    rv = spr_getNew(&pSpr);
    ASSERT_NR(rv == 0);
    (*pppSprs)[i] = pSpr;
    rv = spr_init(pSpr,
             /*x*/112,
             /*y*/194,
          /*offX*/0,
          /*offY*/0,
         /*width*/8,
        /*height*/8,
   /*hitboxWidth*/8,
  /*hitboxHeight*/24,
      /*animData*/_sprGreenStoneData,
       /*animLen*/_sprGreenStoneAnimLen,
          /*type*/SPR_GREEN_STONE
    );
    ASSERT_NR(rv == 0);
    i++;
    pSpr = 0;
    rv = spr_getNew(&pSpr);
    ASSERT_NR(rv == 0);
    (*pppSprs)[i] = pSpr;
    rv = spr_init(pSpr,
             /*x*/136,
             /*y*/178,
          /*offX*/0,
          /*offY*/0,
         /*width*/8,
        /*height*/8,
   /*hitboxWidth*/8,
  /*hitboxHeight*/24,
      /*animData*/_sprCyanStoneData,
       /*animLen*/_sprCyanStoneAnimLen,
          /*type*/SPR_CYAN_STONE
    );
    ASSERT_NR(rv == 0);
    i++;
    pSpr = 0;
    rv = spr_getNew(&pSpr);
    ASSERT_NR(rv == 0);
    (*pppSprs)[i] = pSpr;
    rv = spr_init(pSpr,
             /*x*/168,
             /*y*/162,
          /*offX*/0,
          /*offY*/0,
         /*width*/8,
        /*height*/8,
   /*hitboxWidth*/8,
  /*hitboxHeight*/24,
      /*animData*/_sprBlueStoneData,
       /*animLen*/_sprBlueStoneAnimLen,
          /*type*/SPR_BLUE_STONE
    );
    ASSERT_NR(rv == 0);
    i++;
    pSpr = 0;
    rv = spr_getNew(&pSpr);
    ASSERT_NR(rv == 0);
    (*pppSprs)[i] = pSpr;
    rv = spr_init(pSpr,
             /*x*/192,
             /*y*/162,
          /*offX*/0,
          /*offY*/0,
         /*width*/8,
        /*height*/8,
   /*hitboxWidth*/8,
  /*hitboxHeight*/24,
      /*animData*/_sprPurpleStoneData,
       /*animLen*/_sprPurpleStoneAnimLen,
          /*type*/SPR_PURPLE_STONE
    );
    ASSERT_NR(rv == 0);
    i++;
    rv = 0;
__ret:
    return rv;
}
/** Get all this map's walls into a GFraMe_object buffer */
int map001_getSpikes(sprite ***pppSprs, int *pLen, int *pUsed) {
    int i, len, rv;
    sprite *pSpr;
    
    len = 1;
    
    if (!pppSprs)
        return 1;
    if (!pLen)
        return 1;
    if (!pUsed)
        return 1;
    
    if (*pLen < len) {
        *pppSprs = (sprite**)realloc(*pppSprs, sizeof(sprite*)*len);
        if (!(*pppSprs))
            return 1;
        *pLen = len;
    }
    *pUsed = len;
    
    memset(*pppSprs, 0, sizeof(sprite*)*len);
    
    i = 0;
    pSpr = 0;
    rv = spr_getNew(&pSpr);
    ASSERT_NR(rv == 0);
    (*pppSprs)[i] = pSpr;
    rv = spr_init(pSpr,
             /*x*/248,
             /*y*/219,
          /*offX*/0,
          /*offY*/0,
         /*width*/0,
        /*height*/0,
   /*hitboxWidth*/24,
  /*hitboxHeight*/8,
      /*animData*/0,
       /*animLen*/0,
          /*type*/SPR_SPIKE
    );
    ASSERT_NR(rv == 0);
    i++;
    rv = 0;
__ret:
    return rv;
}
