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
#include <GFraMe/GFraMe_pointer.h>
#include <GFraMe/GFraMe_sprite.h>
#include <GFraMe/GFraMe_spriteset.h>
#include <GFraMe/GFraMe_util.h>

#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "player.h"
#include "sprite.h"

enum { SPR_ANIM_DEF, SPR_ANIM_WALK, SPR_ANIM_JUMP, SPR_ANIM_LASER };

static int _pl_animData[] = 
{
/*fps,loop,num,frames...*/
   8 ,  1 , 12, 80,80,80,80,81,80,82,80,80,80,80,83,
   8 ,  1 , 8 , 84,85,86,87,84,85,88,87,
   0 ,  0 , 1 , 82,
   0 ,  0 , 1 , 89,
};

static int _pl_animLen = 4;

/** 'Export' the player structure */
struct stPlayer {
    /** The player's sprite */
    sprite *pSpr;
    /** Which stones were gotten */
    sprType stones;
    /** Time until next shot */
    int bulCooldown;
    /** Maximun time until next shot; Decreases after each stone */
    int maxBulCooldown;
    /** For how long can one shot before falling to the ground */
    int laserTimer;
    /** Max time for lase; is increased after each stone */
    int maxLaserTimer;
    /** Whether the player is shooting */
    int isShooting;
    /** Bullet's base horizontal speed */
    int bulHorSpeed;
    /** Bullet's base vertical speed */
    int bulVerSpeed;
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
    
    rv = spr_init(pPl->pSpr, x, y, -6/*offX*/, -6/*offY*/, 16/*width*/,
        16/*height*/, 4/*hitboxWidth*/, 10/*hitboxHeight*/, _pl_animData,
        _pl_animLen, SPR_PLAYER);
    ASSERT_NR(rv == 0);
    // Set a hook to itself
    spr_setSuper(pPl->pSpr, (void*)pPl);
    
    pPl->maxBulCooldown = PL_BUL_COOLDOWN;
    pPl->maxLaserTimer = PL_LASER_COOLDOWN;
    
    pPl->bulCooldown = 0;
    pPl->laserTimer = pPl->maxLaserTimer;
    
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
void pl_collideAgainstSprGroup(player *pPl, sprite **pSprs, int sprsLen,
        int isPlFixed, int isSprsFixed) {
    spr_collideAgainstSprGroup(pPl->pSpr, pSprs, sprsLen, isPlFixed, isSprsFixed);
}

/**
 * Give another stone to the player
 */
void pl_addStone(player *pPl, sprType type) {
    pPl->stones |= type;
    pPl->maxBulCooldown -= PL_BUL_DEC;
    pPl->maxLaserTimer += PL_LASER_INC;
}

/**
 * Get all params needed to shot
 */
void pl_getShotParams(int *iniX, int *iniY, int *sX, int *sY, sprType *stones,
        player *pPl) {
    GFraMe_sprite *pGfmSpr;
    
    spr_getSprite(&pGfmSpr, pPl->pSpr);
    // Set this at the 'player's center'
    *iniX = pGfmSpr->obj.x;
    if (pGfmSpr->flipped)
        *iniX -= 6;
    else
        *iniX += 8;
    *iniY = pGfmSpr->obj.y - 1;
    // Set the speed
    *sX = pPl->bulHorSpeed;
    *sY = pPl->bulVerSpeed;
    // Return the amount of stone gotten
    *stones = pPl->stones;
}

/**
 * Returns whether the player is shooting or not (1 on true)
 */
int pl_isShooting(player *pPl) {
    return pPl->isShooting;
}

/**
 * Get the player's center
 */
void pl_getCenter(int *x, int *y, player *pPl) {
    GFraMe_sprite *pGfmSpr;
    GFraMe_object *pObj;
    
    spr_getSprite(&pGfmSpr, pPl->pSpr);
    pObj = &pGfmSpr->obj;
    
    *x = pObj->x + pObj->hitbox.cx;
    *y = pObj->y + pObj->hitbox.cy;
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
void pl_update(player *pPl, camera *pCam, int ms) {
    GFraMe_object *pObj;
    GFraMe_sprite *pSpr;
    int isTouchingDown, isTouchingUp, isLeft, isRight, isJump;
    
    // Get something we can work with
    spr_getSprite(&pSpr, pPl->pSpr);
    pObj = &(pSpr->obj);
    
    if (pPl->bulCooldown > 0)
        pPl->bulCooldown -= ms;
    
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
        if (isLeft) {
            pObj->vx = -PL_VX;
            pSpr->flipped = 1;
        }
        else if (isRight) {
            pObj->vx = PL_VX;
            pSpr->flipped = 0;
        }
        else {
            pObj->vx = 0;
        }
        pPl->laserTimer = pPl->maxLaserTimer;
    }
    if (isJump && isTouchingDown) {
        pObj->vy = -PL_VY;
    }
    if (isTouchingUp) {
        pObj->vy = 0;
    }
    
    // Check if is shooting
    if (pPl->stones != 0 && pPl->laserTimer > 0 && pPl->bulCooldown <= 0 && GFraMe_pointer_pressed) {
        double norm, x, y;
        int ix, iy;
        
        pPl->bulCooldown += pPl->maxBulCooldown;
        pPl->laserTimer -= pPl->maxBulCooldown;
        pPl->isShooting = 1;
        
        ix = GFraMe_pointer_x;
        iy = GFraMe_pointer_y;
        cam_screenToWorld(&ix, &iy, pCam);
        
        x = (double)(ix - pObj->x - pObj->hitbox.cx);
        y = (double)(iy - pObj->y - pObj->hitbox.cy);
        
        norm = ((double)PL_BUL_SPEED) / GFraMe_util_sqrtd(x*x + y*y);
        
        pPl->bulHorSpeed = (int)(norm*x);
        pPl->bulVerSpeed = (int)(norm*y);
        
        if (isTouchingDown && (isLeft || isRight))
            pObj->vx -= pPl->bulHorSpeed;
        else
            pObj->vx = -pPl->bulHorSpeed;
        pObj->vy = -pPl->bulVerSpeed;
    }
    else if (pPl->stones != 0 && pPl->laserTimer > 0 && pPl->bulCooldown <= 0 && GFraMe_controller_max > 0 &&
            (GFraMe_controllers[0].l2 || GFraMe_controllers[0].r2)) {
        double norm, x, y;
        
        pPl->bulCooldown += pPl->maxBulCooldown;
        pPl->laserTimer -= pPl->maxBulCooldown;
        pPl->isShooting = 1;
        
        x = GFraMe_controllers[0].rx;
        y = GFraMe_controllers[0].ry;
        
        norm = ((double)PL_BUL_SPEED) / GFraMe_util_sqrtd(x*x + y*y);
        
        pPl->bulHorSpeed = (int)(norm*x);
        pPl->bulVerSpeed = (int)(norm*y);
        
        if (isTouchingDown && (isLeft || isRight))
            pObj->vx -= pPl->bulHorSpeed;
        else
            pObj->vx = -pPl->bulHorSpeed;
        pObj->vy = -pPl->bulVerSpeed;
    }
    else {
        pPl->isShooting = 0;
    }
    
    if (pPl->isShooting)
        spr_setAnim(pPl->pSpr, SPR_ANIM_LASER, 0/*doReset*/);
    else if (!isTouchingDown)
        spr_setAnim(pPl->pSpr, SPR_ANIM_JUMP, 0/*doReset*/);
    else if (isLeft || isRight)
        spr_setAnim(pPl->pSpr, SPR_ANIM_WALK, 0/*doReset*/);
    else
        spr_setAnim(pPl->pSpr, SPR_ANIM_DEF, 0/*doReset*/);
    
    spr_update(pPl->pSpr, ms);
}

