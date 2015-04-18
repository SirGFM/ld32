/**
 * @file src/sprite.c
 * 
 * Generic animated sprite
 */
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_hitbox.h>
#include <GFraMe/GFraMe_object.h>
#include <GFraMe/GFraMe_sprite.h>
#include <GFraMe/GFraMe_spriteset.h>

#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "sprite.h"

/** 'Export' the sprite structure */
struct stSprite {
    /** The sprite's sprite */
    GFraMe_sprite *pSelf;
    /** The sprite's animations */
    GFraMe_animation *pAnims;
    /** How many animations there are */
    int animLen;
    /** Current's animation index */
    int curAnim;
    /** If the animation changed on this frame */
    int didChangeFrame;
    int lastFrame;
};

/**
 * Alloc a new sprite
 */
int spr_getNew(sprite **ppSpr) {
    int rv;
    
    // Check params
    ASSERT(ppSpr, 1);
    ASSERT(!(*ppSpr), 1);
    
    // Alloc the sprite
    *ppSpr = (sprite*)malloc(sizeof(sprite));
    ASSERT(*ppSpr, 1);
    
    // Clean every variable
    memset(*ppSpr, 0, sizeof(sprite));
    
    // Alloc every needed variable
    (*ppSpr)->pSelf = (GFraMe_sprite*)malloc(sizeof(GFraMe_sprite));
    ASSERT((*ppSpr)->pSelf, 1);
    memset((*ppSpr)->pSelf, 0x0, sizeof(GFraMe_sprite));
    
    rv = 0;
__ret:
    return rv;
}

/**
 * Free a sprite's memory
 */
void spr_free(sprite **ppSpr) {
    // Check params
    ASSERT_NR(ppSpr);
    ASSERT_NR(*ppSpr);
    
    if ((*ppSpr)->pSelf) {
        free((*ppSpr)->pSelf);
        (*ppSpr)->pSelf = 0;
    }
    if ((*ppSpr)->pAnims) {
        free((*ppSpr)->pAnims);
        (*ppSpr)->pAnims = 0;
    }
    
    // Free the sprite
    free(*ppSpr);
    *ppSpr = 0;
    
__ret:
    return;
}

/**
 * Initializes a sprite and its animations; The first one will be run;
 * 
 * The 'animData' array should contain data in the format:
 *   fps | doLoop | frameCount | frames[frameCount]
 */
int spr_init(sprite *pSpr, int x, int y, int offX, int offY, int width,
        int height, int hitboxWidth, int hitboxHeight, int *animData,
        int animLen) {
    int rv, i;
    GFraMe_spriteset *pSset;
    
    // Check the arguments
    ASSERT(pSpr, 1);
    ASSERT(pSpr->pSelf, 1);
    ASSERT(animData, 1);
    ASSERT(animLen > 0, 1);
    
    // Select the correct spriteset
    if (width == 2 && height == 2)
        pSset = gl_sset2x2;
    else if (width == 8 && height == 8)
        pSset = gl_sset8x8;
    else if (width == 16 && height == 16)
        pSset = gl_sset16x16;
    else
        return 1;
    
    GFraMe_sprite_init(pSpr->pSelf, x, y, hitboxWidth, hitboxHeight, pSset,
            offX, offY);
    
    if (animLen > pSpr->animLen) {
        pSpr->pAnims = (GFraMe_animation*)realloc(pSpr->pAnims,
                sizeof(GFraMe_animation)*animLen);
        ASSERT(pSpr->pAnims, 1);
        memset(pSpr->pAnims, 0, sizeof(GFraMe_animation)*animLen);
        pSpr->animLen = animLen;
    }
    
    i = 0;
    while (i < animLen) {
        int fps, doLoop, frameCount, *frames;
        
        // Get the animation's data
        fps        =   animData[i + 0];
        doLoop     =   animData[i + 1];
        frameCount =   animData[i + 2];
        frames     = &(animData[i + 3]);
        
        // Initialize it
        GFraMe_animation_init(&pSpr->pAnims[i], fps, frames, frameCount,
                doLoop);
        
        i += frameCount + 3;
    }
    
    // Play the first animation
    pSpr->curAnim = -1;
    spr_setAnim(pSpr, 0, 1);
    
    rv = 0;
__ret:
    return rv;
}

/**
 * Set the sprite's animation
 */
void spr_setAnim(sprite *pSpr, int anim, int doRestart) {
    // Check the arguments
    ASSERT_NR(pSpr);
    ASSERT_NR(pSpr->pSelf);
    ASSERT_NR(pSpr->pAnims);
    ASSERT_NR(pSpr->curAnim != anim);
    ASSERT_NR(pSpr->animLen > anim);
    
    GFraMe_sprite_set_animation(pSpr->pSelf, &pSpr->pAnims[anim], !doRestart);
__ret:
    return;
}

/**
 * Returns whether a frame change just happened or not (1 if true)
 */
int spr_didChangeFrame(sprite *pSpr) {
    return pSpr->didChangeFrame;
}

/**
 * Returns whether an animation did finish (1 if true)
 */
int spr_didAnimationFinish(sprite *pSpr) {
    return pSpr->pSelf->anim == 0;
}

/**
 * Draw the sprite
 */
void spr_draw(sprite *pSpr, camera *pCam) {
    int camX, camY, camW, camH;
    
    cam_getParams(&camX, &camY, &camW, &camH, pCam);
    GFraMe_sprite_draw_camera(pSpr->pSelf, camX, camY, camW, camH);
}

/**
 * Updated the sprite
 */
void spr_update(sprite *pSpr, int ms) {
    pSpr->didChangeFrame = 0;
    
    GFraMe_sprite_update(pSpr->pSelf, ms);
    
    if (pSpr->lastFrame != pSpr->pSelf->cur_tile) {
        pSpr->lastFrame = pSpr->pSelf->cur_tile;
        pSpr->didChangeFrame = 1;
    }
}

/**
 * Get the lib's sprite
 */
void spr_getSprite(GFraMe_sprite **ppSpr, sprite *pSpr) {
    *ppSpr = pSpr->pSelf;
}

