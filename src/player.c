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

#include "audio.h"
#include "global.h"
#include "player.h"
#include "sprite.h"

enum { SPR_ANIM_DEF, SPR_ANIM_WALK, SPR_ANIM_JUMP, SPR_ANIM_LASER, SPR_ANIM_DEATH };

static int _pl_animData[] = 
{
/*fps,loop,num,frames...*/
   8 ,  1 , 12, 80,80,80,80,81,80,82,80,80,80,80,83,
   8 ,  1 , 8 , 84,85,86,87,84,85,88,87,
   0 ,  0 , 1 , 82,
   0 ,  0 , 1 , 89,
   2 ,  0 , 1 , 90,
};

static int _pl_animLen = 5;

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
    /** Checkpoint's position */
    int checkpointX;
    /** Checkpoint's position */
    int checkpointY;
    /** Whether the fall sfx has player */
    int didPlayFall;
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
    
    pPl->checkpointX = x;
    pPl->checkpointY = y;
    
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
    if (!(pPl->stones & type)) {
        GFraMe_sprite *pGfmSpr;
        
        spr_getSprite(&pGfmSpr, pPl->pSpr);
        
        pPl->stones |= type;
        pPl->maxBulCooldown -= PL_BUL_DEC;
        pPl->maxLaserTimer += PL_LASER_INC;
        
        pPl->checkpointX = pGfmSpr->obj.x;
        pPl->checkpointY = pGfmSpr->obj.y;
        
        aud_playPlGetStone();
    }
}

/**
 * Set a checkpoint
 */
void pl_setCheckpoint(player *pPl) {
    GFraMe_sprite *pGfmSpr;
    
    spr_getSprite(&pGfmSpr, pPl->pSpr);
    
    pPl->checkpointX = pGfmSpr->obj.x;
    pPl->checkpointY = pGfmSpr->obj.y;
}

/** 
 * Returns whether the player is alive
 */
int pl_isAlive(player *pPl) {
    return spr_isAlive(pPl->pSpr);
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
 * Kill the player
 */
void pl_kill(player *pPl) {
    if (spr_isAlive(pPl->pSpr)) {
        aud_playPlDeath();
    }
    spr_setAnim(pPl->pSpr, SPR_ANIM_DEATH, 1/*doReset*/);
    spr_kill(pPl->pSpr);
}

/**
 * Revives the player at the last checkpoint
 */
void pl_revive(player *pPl) {
    GFraMe_sprite *pGfmSpr;
    GFraMe_object *pObj;
    
    spr_getSprite(&pGfmSpr, pPl->pSpr);
    pObj = &pGfmSpr->obj;
    
    GFraMe_object_set_pos(pObj, pPl->checkpointX, pPl->checkpointY);
    spr_revive(pPl->pSpr);
    
    aud_playPlRevive();
}

/** * Get info to say for how long can still shoot
 */
void pl_getShotInfo(double *laserDur, sprType *stones, player *pPl) {
    *laserDur = (double)pPl->laserTimer / (double)pPl->maxLaserTimer;
    *stones = pPl->stones;
}

/**
 * Get how many stones the player has
 */
void pl_getStoneCount(int *pNum, player *pPl) {
    int count;
    sprType tmp;
    
    tmp = pPl->stones;
    count = 0;
    while (tmp > 0) {
        if (tmp & 1)
            count++;
        tmp >>= 1;
    }
    
    *pNum = count;
}

/**
 * Renders the player
 */
void pl_draw(player *pPl, camera *pCam) {
    int doKill = 0;
    
    if (!spr_isAlive(pPl->pSpr)) {
        spr_revive(pPl->pSpr);
        doKill = 1;
    }
    
    spr_draw(pPl->pSpr, pCam);
    
    // Draw the target
    if (GFraMe_controller_max > 0 && (GFraMe_controllers[0].rx > 0.3 ||
            GFraMe_controllers[0].rx < -0.3 || GFraMe_controllers[0].ry > 0.3 ||
            GFraMe_controllers[0].ry < -0.3)) {
        GFraMe_sprite *pGfmSpr;
        int tile, x, y, cx, cy;
        
        spr_getSprite(&pGfmSpr, pPl->pSpr);
        cam_getPos(&cx, &cy, pCam);
        
        tile = 295;
        x = pGfmSpr->obj.x + GFraMe_controllers[0].rx * PL_TARGET_DIST - cx;
        if (pGfmSpr->flipped)
            x -= 6;
        else
            x += 8;
        y = pGfmSpr->obj.y - 1 + GFraMe_controllers[0].ry * PL_TARGET_DIST - cy;
        
        GFraMe_spriteset_draw(gl_sset8x8, tile, x, y, 0/*flipped*/);
        
    }
    
    if (doKill)
        spr_kill(pPl->pSpr);
}

/**
 * Updates the player
 */
void pl_update(player *pPl, camera *pCam, int ms) {
    GFraMe_object *pObj;
    GFraMe_sprite *pSpr;
    int isTouchingDown, isTouchingUp, isLeft, isRight, isJump;
    
    if (spr_getAnim(pPl->pSpr) == SPR_ANIM_DEATH) {
        return;
    }
    
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
        if (!pPl->didPlayFall) {
            aud_playPlFall();
            pPl->didPlayFall = 1;
        }
    }
    else {
        pObj->ay = GRAV;
        pPl->didPlayFall = 0;
    }
    
    // Check if is pressing left
    isLeft = GFraMe_keys.a;
    isLeft = isLeft || GFraMe_keys.q;
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
        aud_playPlJump();
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
        
        if (pObj->vx > 0)
            pSpr->flipped = 1;
        else if (pObj->vx < 0)
            pSpr->flipped = 0;
        
        aud_playBlBullet();
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
        
        if (pObj->vx > 0)
            pSpr->flipped = 1;
        else if (pObj->vx < 0)
            pSpr->flipped = 0;
        
        aud_playBlBullet();
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
    
    if (spr_didChangeFrame(pPl->pSpr) && (pSpr->cur_tile == 84 ||
            pSpr->cur_tile == 86 || pSpr->cur_tile == 88)) {
        aud_playPlStep();
    }
}

