/**
 * @file src/player.h
 * 
 * Player module
 */
#include <GFraMe/GFraMe_controller.h>
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_keys.h>
#include <GFraMe/GFraMe_hitbox.h>
#include <GFraMe/GFraMe_object.h>
#include <GFraMe/GFraMe_sprite.h>
#include <GFraMe/GFraMe_spriteset.h>

#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "player.h"
#include "sprite.h"

enum { SPR_ANIM_DEF, SPR_ANIM_WALK };

static int _pl_animData[] = 
{
/*fps,loop,num,frames...*/
   0 ,  0 , 1 , 80,
   12,  1 , 8 , 81,82,83,84,85,86,87,88,
};

static int _pl_animLen = 2;

/** 'Export' the player structure */
struct stPlayer {
    /** The player's sprite */
    sprite *pSpr;
};

/**
 * Alloc a new player
 */
int pl_getNew(player **ppPl) {
    int rv;
    
    // Check params
    ASSERT(ppPl, 1);
    ASSERT(!(*ppPl), 1);
    
    // Alloc the player
    *ppPl = (player*)malloc(sizeof(player));
    ASSERT(*ppPl, 1);
    
    // Clean every variable
    memset(*ppPl, 0, sizeof(player));
    
    // Alloc every needed variable
    rv = spr_getNew(&((*ppPl)->pSpr));
    ASSERT_NR(rv == 0);
    
    rv = 0;
__ret:
    return rv;
}

/**
 * Free a player's memory
 */
void pl_free(player **ppPl) {
    // Check params
    ASSERT_NR(ppPl);
    ASSERT_NR(*ppPl);
    
    if ((*ppPl)->pSpr)
        spr_free((&(*ppPl)->pSpr));
    
    // Free the player
    free(*ppPl);
    *ppPl = 0;
    
__ret:
    return;
}

/**
 * Initialize the player and its position
 */
int pl_init(player *pPl, int x, int y) {
    int rv;
    
    // Check the params
    ASSERT(pPl, 1);
    ASSERT(pPl->pSpr, 1);
    
    rv = spr_init(pPl->pSpr, x, y, -6/*offX*/, -4/*offY*/, 16/*width*/,
        16/*height*/, 4/*hitboxWidth*/, 12/*hitboxHeight*/, _pl_animData,
        _pl_animLen, SPR_PLAYER);
    ASSERT_NR(rv == 0);
    
    rv = 0;
__ret:
    return rv;
}

/**
 * Collides a player against various objects
 */
void pl_collideAgainstGroup(player *pPl, GFraMe_object *pObjs, int objsLen,
        int isPlFixed, int isObjsFixed) {
    spr_collideAgainstGroup(pPl->pSpr, pObjs, objsLen, isPlFixed, isObjsFixed);
}

/**
 * Collides a player against various sprites
 */
void pl_collideAgainstSprGroup(player *pPl, sprite *pSprs, int sprsLen,
        int isPlFixed, int isSprsFixed) {
    spr_collideAgainstSprGroup(pPl->pSpr, pSprs, sprsLen, isPlFixed, isSprsFixed);
}

/**
 * Renders the player
 */
void pl_draw(player *pPl, camera *pCam) {
    spr_draw(pPl->pSpr, pCam);
}

/**
 * Updates the player
 */
void pl_update(player *pPl, int ms) {
    GFraMe_object *pObj;
    GFraMe_sprite *pSpr;
    int isTouchingDown, isTouchingUp, isLeft, isRight, isJump;
    
    // Get something we can work with
    spr_getSprite(&pSpr, pPl->pSpr);
    pObj = &(pSpr->obj);
    
    isTouchingDown = (pObj->hit & GFraMe_direction_down) != 0;
    isTouchingUp = (pObj->hit & GFraMe_direction_up) != 0;
    if (isTouchingDown) {
        // If on the ground, reset the speed to keep the player touching the floor
        pObj->vy = PL_VY;
    }
    else {
        pObj->ay = GRAV;
    }
    
    // Check if is pressing left
    isLeft = GFraMe_keys.a;
    isLeft = isLeft || (GFraMe_controller_max > 0 &&
            GFraMe_controllers[0].left);
    isLeft = isLeft || (GFraMe_controller_max > 0 &&
            GFraMe_controllers[0].lx < -0.3);
    // Check if is pressing right
    isRight = GFraMe_keys.d;
    isRight = isRight || (GFraMe_controller_max > 0 &&
            GFraMe_controllers[0].right);
    isRight = isRight || (GFraMe_controller_max > 0 &&
            GFraMe_controllers[0].lx > 0.3);
    // Check if is jumping
    isJump = GFraMe_keys.space;
    isJump = isJump || (GFraMe_controller_max > 0 && GFraMe_controllers[0].r1);
    isJump = isJump || (GFraMe_controller_max > 0 && GFraMe_controllers[0].l1);
    
    // Movement
    if (isTouchingDown) {
        spr_setAnim(pPl->pSpr, SPR_ANIM_WALK, 0/*doReset*/);
        if (isLeft) {
            pObj->vx = -PL_VX;
            pSpr->flipped = 1;
        }
        else if (isRight) {
            pObj->vx = PL_VX;
            pSpr->flipped = 0;
        }
        else {
            spr_setAnim(pPl->pSpr, SPR_ANIM_DEF, 0/*doReset*/);
            pObj->vx = 0;
        }
    }
    if (isJump && isTouchingDown) {
        pObj->vy = -PL_VY;
    }
    if (isTouchingUp) {
        pObj->vy = 0;
    }
    
    spr_update(pPl->pSpr, ms);
}

