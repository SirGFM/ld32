/**
 * @file new-src/player.c
 * 
 * The player
 */
#include <GFraMe/gframe.h>
#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmCamera.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmParser.h>
#include <GFraMe/gfmSprite.h>

#include <ld32_pc/game.h>
#include <ld32_pc/player.h>

#include <stdlib.h>
#include <string.h>

enum {
    SPR_ANIM_DEF,
    SPR_ANIM_WALK,
    SPR_ANIM_JUMP,
    SPR_ANIM_LASER,
    SPR_ANIM_DEATH
};

static int _pl_animData[] = 
{
/*              */ /*len|fps|loop|frames...*/
/* SPR_ANIM_DEF */    12, 8 , 1  , 80,80,80,80,81,80,82,80,80,80,80,83,
/*SPR_ANIM_WALK */    8 , 8 , 1  , 84,85,86,87,84,85,88,87,
/*SPR_ANIM_JUMP */    1 , 0 , 0  , 82,
/*SPR_ANIM_LASER*/    1 , 0 , 0  , 89,
/*SPR_ANIM_DEATH*/    1 , 2 , 0  , 90,
};

struct stPlayer {
    gfmSprite *pSpr;
    int curAnim;
    /** For how long the camera has been tweening (in the range [-500,500]ms) */
    int cameraTweenTime;
};

/**
 * Change the currently playing animation
 * 
 * @param [in]pCtx The player
 * @param [in]anim The animation's index
 */
static gfmRV stPl_setAnimation(player *pCtx, int anim) {
    gfmRV rv;
    
    if (pCtx->curAnim == anim) {
        return GFMRV_OK;
    }
    
    rv = gfmSprite_playAnimation(pCtx->pSpr, anim);
    ASSERT(rv == GFMRV_OK, rv);
    
    pCtx->curAnim = anim;
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Initialize a player
 * 
 * @param  [out]ppCtx   The player
 * @param  [in] pGame   The game context (to retrieve the texture and spriteset)
 * @param  [in] pParser The parser context (to retrieve the player's attributes)
 * @return              GFMRV_OK, GFMRV_ALLOC_FAILED, ...
 */
gfmRV pl_init(player **ppCtx, gameCtx *pGame, gfmParser *pParser) {
    gfmRV rv;
    player *pCtx;
    int x, y;
    
    // Retrieve the player's attributes from the parser
    rv = gfmParser_getPos(&x, &y, pParser);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Alloc the player
    pCtx = (player*)malloc(sizeof(player));
    ASSERT(pCtx, GFMRV_ALLOC_FAILED);
    memset(pCtx, 0x0, sizeof(player));
    
    // Initialize its sprite
    rv = gfmSprite_getNew(&(pCtx->pSpr));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_init(pCtx->pSpr, x, y, 4/*width*/, 10/*height*/,
            pGame->pSset16x16, -6/*offX*/, -6/*offY*/, pCtx/*pChild*/,
            tPlayer/*type*/);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Load and set its animations
    rv = gfmSprite_addAnimationsStatic(pCtx->pSpr, _pl_animData);
    ASSERT(rv == GFMRV_OK, rv);
    
    pCtx->curAnim = -1;
    rv = stPl_setAnimation(pCtx, SPR_ANIM_DEF);
    ASSERT(rv == GFMRV_OK, rv);
    
    *ppCtx = pCtx;
    pCtx = 0;
    rv = GFMRV_OK;
__ret:
    // If this pointer wasn't cleared, something happened and it must be dealloc
    if (pCtx) {
        pl_clean(&pCtx);
    }
    return rv;
}

/**
 * Free all memory used by the player
 * 
 * @param  [in|out]ppCtx The player
 */
void pl_clean(player **ppCtx) {
    if (!ppCtx || !(*ppCtx))
        return;
    
    gfmSprite_free(&((*ppCtx)->pSpr));
    
    free(*ppCtx);
    *ppCtx = 0;
}

/**
 * Update the player
 * 
 * @param  [in]pCtx The player
 * @return          GFMRV_OK, ...
 */
gfmRV pl_update(player *pPlayer, gameCtx *pGame) {
    gfmCamera *pCamera;
    gfmCtx *pCtx;
    gfmRV rv;
    int elapsed;
    
    pCtx = pGame->pCtx;
    rv = gfm_getElapsedTime(&elapsed, pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_getCamera(&pCamera, pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    
    /** == Pre-update (handle inputs) ======================================= */
    
    // Temp stuff for testing...
    if (pGame->stRight & gfmInput_pressed) {
        rv = gfmSprite_setDirection(pPlayer->pSpr, 0/*isFlipped*/);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmSprite_setHorizontalVelocity(pPlayer->pSpr, 100/*vx*/);
        ASSERT(rv == GFMRV_OK, rv);
    }
    else if (pGame->stLeft & gfmInput_pressed) {
        rv = gfmSprite_setDirection(pPlayer->pSpr, 1/*isFlipped*/);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmSprite_setHorizontalVelocity(pPlayer->pSpr, -100/*vx*/);
        ASSERT(rv == GFMRV_OK, rv);
    }
    else {
        rv = gfmSprite_setHorizontalVelocity(pPlayer->pSpr, 0/*vx*/);
        ASSERT(rv == GFMRV_OK, rv);
    }
    
    /** == Update the physics =============================================== */
    rv = gfmSprite_update(pPlayer->pSpr, pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    
    /** == Post-update (animation, move camera, etc) ======================== */
    
    // Update the camera deadzone and position
    do {
        int cx, cy, isFlipped;
        int border, h, w, x, y;
        int alpha;
        
        border = 40;
        h = GAME_BBUF_HEIGHT - GAME_UI_HEIGHT;
        w = 20;
        y = 0;
        
        rv = gfmSprite_getDirection(&isFlipped, pPlayer->pSpr);
        ASSERT(rv == GFMRV_OK, rv);
        if (!isFlipped && pPlayer->cameraTweenTime > -500) {
            // If the player is facing right and the camera isn't to the left
            pPlayer->cameraTweenTime -= elapsed / pGame->deadzoneSpeed ;
            if (pPlayer->cameraTweenTime < -500) {
                pPlayer->cameraTweenTime = -500;
            }
        }
        else if (isFlipped && pPlayer->cameraTweenTime < 500) {
            // If the player is facing left and the camera isn't to the right
            pPlayer->cameraTweenTime += elapsed / pGame->deadzoneSpeed ;
            if (pPlayer->cameraTweenTime > 500) {
                pPlayer->cameraTweenTime = 500;
            }
        }
        alpha = pPlayer->cameraTweenTime + 500;
        x = (GAME_BBUF_WIDTH - border * 2 - w) * alpha / 1000.0f + border;
        
        rv = gfmCamera_setDeadzone(pCamera, x, y, w, h);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmSprite_getCenter(&cx, &cy, pPlayer->pSpr);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmCamera_centerAtPoint(pCamera, cx, cy);
        ASSERT(rv == GFMRV_CAMERA_MOVED || rv == GFMRV_CAMERA_DIDNT_MOVE, rv);
    } while (0);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Render the player
 * 
 * @param  [in]pCtx The player
 * @return          GFMRV_OK, ...
 */
gfmRV pl_draw(player *pPlayer, gameCtx *pGame) {
    return gfmSprite_draw(pPlayer->pSpr, pGame->pCtx);
}

