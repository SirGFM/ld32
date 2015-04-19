/**
 * @file src/sprite.h
 * 
 * Generic animated sprite
 */
#ifndef __SPRITE_H__
#define __SPRITE_H__

#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_sprite.h>

#include "camera.h"

extern int _sprRedStoneData[];
extern int _sprRedStoneAnimLen;
extern int _sprOrangeStoneData[];
extern int _sprOrangeStoneAnimLen;
extern int _sprYellowStoneData[];
extern int _sprYellowStoneAnimLen;
extern int _sprGreenStoneData[];
extern int _sprGreenStoneAnimLen;
extern int _sprCyanStoneData[];
extern int _sprCyanStoneAnimLen;
extern int _sprBlueStoneData[];
extern int _sprBlueStoneAnimLen;
extern int _sprPurpleStoneData[];
extern int _sprPurpleStoneAnimLen;

/** 'Export' the sprite structure */
typedef struct stSprite sprite;

typedef enum {
    SPR_PLAYER       = 0x00000001,
    SPR_RED_STONE    = 0x00000002,
    SPR_ORANGE_STONE = 0x00000004,
    SPR_YELLOW_STONE = 0x00000008,
    SPR_GREEN_STONE  = 0x00000010,
    SPR_CYAN_STONE   = 0x00000020,
    SPR_BLUE_STONE   = 0x00000040,
    SPR_PURPLE_STONE = 0x00000080,
    SPR_TYPES_MAX
} sprType;

/**
 * Alloc a new sprite
 */
int spr_getNew(sprite **ppSpr);

/**
 * Free a sprite's memory
 */
void spr_free(sprite **ppSpr);

/**
 * Initializes a sprite and its animations; The first one will be run;
 * 
 * The 'animData' array should contain data in the format:
 *   fps | doLoop | frameCount | frames[frameCount]
 */
int spr_init(sprite *pSpr, int x, int y, int offX, int offY, int width,
        int height, int hitboxWidth, int hitboxHeight, int *animData,
        int animLen, sprType type);

/**
 * Assign something as this' super
 */
void spr_setSuper(sprite *pSpr, void *pObj);

/**
 * Get the super
 */
void spr_getSuper(void **pObj, sprite *pSpr);

/**
 * Set the sprite's animation
 */
void spr_setAnim(sprite *pSpr, int anim, int doRestart);

/**
 * Returns whether a frame change just happened or not (1 if true)
 */
int spr_didChangeFrame(sprite *pSpr);

/**
 * Returns whether an animation did finish (1 if true)
 */
int spr_didAnimationFinish(sprite *pSpr);

/**
 * Draw the sprite
 */
void spr_draw(sprite *pSpr, camera *pCam);

/**
 * Updated the sprite
 */
void spr_update(sprite *pSpr, int ms);

/**
 * Get the lib's sprite
 */
void spr_getSprite(GFraMe_sprite **ppSpr, sprite *pSpr);

/**
 * Collides a sprite against various objects
 */
void spr_collideAgainstGroup(sprite *pSpr, GFraMe_object *pObjs, int objsLen,
        int isSprFixed, int isObjsFixed);

/**
 * Collides a sprite against various sprites
 */
void spr_collideAgainstSprGroup(sprite *pSpr, sprite **pSprs, int sprsLen,
        int isSprFixed, int isSprsFixed);

/**
 * Set this sprite as not active
 */
void spr_kill(sprite *pSpr);

#endif /* __SPRITE_H__ */

