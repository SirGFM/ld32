/**
 * @file src/sprite.h
 * 
 * Generic animated sprite
 */
#ifndef __SPRITE_H__
#define __SPRITE_H__

#include <GFraMe/GFraMe_error.h>

#include "camera.h"

/** 'Export' the sprite structure */
typedef struct stSprite sprite;

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
        int animLen);

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

#endif /* __SPRITE_H__ */

