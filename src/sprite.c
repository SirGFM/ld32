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

#include "collision.h"
#include "global.h"
#include "sprite.h"

int _sprRedStoneData[] = {0,0,1,288};
int _sprRedStoneAnimLen = 1;
int _sprOrangeStoneData[] = {0,0,1,289};
int _sprOrangeStoneAnimLen = 1;
int _sprYellowStoneData[] = {0,0,1,290};
int _sprYellowStoneAnimLen = 1;
int _sprGreenStoneData[] = {0,0,1,291};
int _sprGreenStoneAnimLen = 1;
int _sprCyanStoneData[] = {0,0,1,292};
int _sprCyanStoneAnimLen = 1;
int _sprBlueStoneData[] = {0,0,1,293};
int _sprBlueStoneAnimLen = 1;
int _sprPurpleStoneData[] = {0,0,1,294};
int _sprPurpleStoneAnimLen = 1;

int _sprRedBulAnimData[] = {0,0,1,4096};
int _sprRedBulAnimLen = 1;
int _sprOrangeBulAnimData[] = {0,0,1,4097};
int _sprOrangeBulAnimLen = 1;
int _sprYellowBulAnimData[] = {0,0,1,4098};
int _sprYellowBulAnimLen = 1;
int _sprGreenBulAnimData[] = {0,0,1,4099};
int _sprGreenBulAnimLen = 1;
int _sprCyanBulAnimData[] = {0,0,1,4100};
int _sprCyanBulAnimLen = 1;
int _sprBlueBulAnimData[] = {0,0,1,4101};
int _sprBlueBulAnimLen = 1;
int _sprPurpleBulAnimData[] = {0,0,1,4102};
int _sprPurpleBulAnimLen = 1;

/** 'Export' the sprite structure */
struct stSprite {
    /** Used by the player to set a hook to itself */
    void *pSuper;
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
    /** Previous animation frame */
    int lastFrame;
    /** This sprite's type */
    sprType type;
    /** Whether the sprite is active and should be updated and draw */
    int isActive;
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
 * Try to get a new sprite, first looking into a list
 */
int spr_recycle(sprite **ppSpr, sprite ***pppSpr, int *pLen) {
    int i, j, rv;
    
    ASSERT(ppSpr, 1);
    ASSERT(pppSpr, 1);
    ASSERT(pLen, 1);
    
    // Try to find an unused object
    i = 0;
    while (i < *pLen) {
        if (!(*pppSpr)[i]->isActive) {
            *ppSpr = (*pppSpr)[i];
            return 0;
        }
        i++;
    }
    
    // Otherwise, expand the buffer
    *pLen += 8;
    *pppSpr = (sprite**)realloc(*pppSpr, (*pLen)*sizeof(sprite*));
    ASSERT(*pppSpr, 1);
    
    // Alloc every object
    j = i;
    while (j < *pLen) {
        sprite *pSpr = 0;
        
        rv = spr_getNew(&pSpr);
        ASSERT_NR(rv == 0);
        (*pppSpr)[j] = pSpr;
        
        j++;
    }
    
    // Retrieve the first of the new ones
    *ppSpr = (*pppSpr)[i];
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
        int animLen, sprType type) {
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
        fps        =   animData[0];
        doLoop     =   animData[1];
        frameCount =   animData[2];
        frames     = &(animData[3]);
        
        // Initialize it
        GFraMe_animation_init(&pSpr->pAnims[i], fps, frames, frameCount,
                doLoop);
        
        i++;
        animData += 3 + frameCount;
    }
    
    // Play the first animation
    pSpr->curAnim = -1;
    spr_setAnim(pSpr, 0, 1);
    pSpr->type = type;
    pSpr->isActive = 1;
    
    rv = 0;
__ret:
    return rv;
}

/**
 * Assign something as this' super
 */
void spr_setSuper(sprite *pSpr, void *pObj) {
    pSpr->pSuper = pObj;
}

/**
 * Get the super
 */
void spr_getSuper(void **pObj, sprite *pSpr) {
    *pObj = pSpr->pSuper;
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
    
    if (pSpr->isActive) {
        cam_getParams(&camX, &camY, &camW, &camH, pCam);
        GFraMe_sprite_draw_camera(pSpr->pSelf, camX, camY, camW, camH);
    }
}

/**
 * Updated the sprite
 */
void spr_update(sprite *pSpr, int ms) {
    if (pSpr->isActive) {
        pSpr->didChangeFrame = 0;
        
        GFraMe_sprite_update(pSpr->pSelf, ms);
        
        if (pSpr->lastFrame != pSpr->pSelf->cur_tile) {
            pSpr->lastFrame = pSpr->pSelf->cur_tile;
            pSpr->didChangeFrame = 1;
        }
    }
}

/**
 * Returns whether the sprite is inside the camera
 */
int spr_isInsideCamera(sprite *pSpr, camera *pCam) {
    GFraMe_object *pObj;
    int camX, camY, camW, camH, rv;
    
    ASSERT(pSpr->isActive, 0);
    
    cam_getParams(&camX, &camY, &camW, &camH, pCam);
    pObj = &(pSpr->pSelf->obj);
    
    ASSERT(pObj->x + pObj->hitbox.cx + pObj->hitbox.hw >= camX && pObj->x <= camX + camW, 0);
    ASSERT(pObj->y + pObj->hitbox.cy + pObj->hitbox.hh >= camY && pObj->y <= camY + camH, 0);
    
    rv = 1;
__ret:
    return rv;
}

/**
 * Get the lib's sprite
 */
void spr_getSprite(GFraMe_sprite **ppSpr, sprite *pSpr) {
    *ppSpr = pSpr->pSelf;
}

/**
 * Collides a sprite against various objects
 */
void spr_collideAgainstGroup(sprite *pSpr, GFraMe_object *pObjs, int objsLen,
        int isPlFixed, int isObjsFixed) {
    GFraMe_collision_type mode;
    GFraMe_object *pObj;
    int i;
    
    pObj = &(pSpr->pSelf->obj);
    
    if (isPlFixed && isObjsFixed) {
        mode = GFraMe_collision_full;
    }
    else if (isPlFixed) {
        mode = GFraMe_first_fixed;
    }
    else if (isObjsFixed) {
        mode = GFraMe_second_fixed;
    }
    else {
        mode = GFraMe_dont_collide;
    }
    
    i = 0;
    while (i < objsLen) {
        GFraMe_object_overlap(pObj, &(pObjs[i]), mode);
        i++;
    }
}

/**
 * Collides a sprite against various sprites
 */
void spr_collideAgainstSprGroup(sprite *pSpr, sprite **pSprs, int sprsLen,
        int isSprFixed, int isSprsFixed) {
    GFraMe_collision_type mode;
    GFraMe_object *pThisObj;
    int i;
    
    pThisObj = &(pSpr->pSelf->obj);
    
    if (isSprFixed && isSprsFixed) {
        mode = GFraMe_collision_full;
    }
    else if (isSprFixed) {
        mode = GFraMe_first_fixed;
    }
    else if (isSprsFixed) {
        mode = GFraMe_second_fixed;
    }
    else {
        mode = GFraMe_dont_collide;
    }
    
    i = 0;
    while (i < sprsLen) {
        if (pSprs[i]->isActive) {
            GFraMe_ret rv;
            GFraMe_object *pOtherObj;
            
            pOtherObj = &(pSprs[i]->pSelf->obj);
            
            rv = GFraMe_object_overlap(pThisObj, pOtherObj, mode);
            
            if (rv == GFraMe_ret_ok) {
                collisionCallback(pSpr, pSprs[i], pSpr->type, pSprs[i]->type);
                if (!pSpr->isActive)
                    break;
            }
        }
        
        i++;
    }
}

/**
 * Set this sprite as not active
 */
void spr_kill(sprite *pSpr) {
    pSpr->isActive = 0;
}

