/**
 * @file new-src/player.c
 * 
 * The player
 */
#include <GFraMe/gframe.h>
#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmCamera.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmGroup.h>
#include <GFraMe/gfmInput.h>
#include <GFraMe/gfmParser.h>
#include <GFraMe/gfmSprite.h>

#include <ld32_pc/collision.h>
#include <ld32_pc/game.h>
#include <ld32_pc/player.h>
#include <ld32_pc/playstate.h>

#include <math.h>
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
/* SPR_ANIM_DEF */    12, 8 , 1  , 96,96,96,96,97,96,98,96,96,96,96,99,
/*SPR_ANIM_WALK */    8 , 8 , 1  , 100,101,102,103,100,101,104,103,
/*SPR_ANIM_JUMP */    1 , 0 , 0  , 98,
/*SPR_ANIM_LASER*/    1 , 0 , 0  , 105,
/*SPR_ANIM_DEATH*/    1 , 2 , 0  , 106,
};

static int _tgt_animData[] = 
{
/*              */ /*len|fps|loop|frames...*/
/* TGT_ANIM_DEF */    4 , 8 , 1  , 94,93,92,93
};

struct stPlayer {
    /** Normalized horizontal direction of the target */
    float dx;
    /** Normalized vertical direction of the target */
    float dy;
    gfmSprite *pSpr;
    gfmSprite *pTarget;
    /** Current animation */
    int curAnim;
    /** For how long the camera has been tweening (in the range [-500,500]ms) */
    int cameraTweenTime;
    /** Countdown to respawn the player */
    int respawnTimer;
    /** Default horizontal speed */
    int vx;
    /** Default jump speed */
    int vy;
    /** Which stones have been retrieved */
    int stones;
    /** How many bars there are left */
    int curPower;
    /** How many bars the player has */
    int totalPower;
    /** How long until the next bar */
    int nextShoot;
    /** For how long shooting lasts (until a new bar is used) */
    int shootDelay;
    /** Set the propelling speed */
    int propelSpeed;
    /** Set the max propelling speed */
    int maxPropelSpeed;
    /** How many frames it has been since particles where spawned */
    int lastParticle;
};

/**
 * Give the player a power stone and update its stats
 * 
 * @param  [in]pCtx  The player
 * @param  [in]stone The stones' types
 & @return           GFMRV_OK, ...
 */
static gfmRV stPl_addPower(player *pCtx, int stone) {
    int curStone;
    
    curStone = 1;
    while (stone != 0) {
        // Check if that stone's bit should be set
        if (stone & 1) {
            pCtx->stones |= curStone;
            pCtx->totalPower += 4;
            pCtx->curPower = pCtx->totalPower;
            pCtx->shootDelay += 100;
            pCtx->nextShoot = 0;
            // TODO Tune this value
            pCtx->maxPropelSpeed += 15;
        }
        // Move to the next stone
        curStone <<= 1;
        stone >>= 1;
    }
    
    return GFMRV_OK;
}

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
    rv = gfmSprite_init(pCtx->pSpr, x, y - 16, 4/*width*/, 10/*height*/,
            pGame->pSset16x16, -6/*offX*/, -6/*offY*/, pCtx/*pChild*/,
            tPlayer/*type*/);
    ASSERT(rv == GFMRV_OK, rv);
    
    rv = gfmSprite_getNew(&(pCtx->pTarget));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_init(pCtx->pTarget, -100, -100, 16/*width*/, 16/*height*/,
            pGame->pSset16x16, -8/*offX*/, -8/*offY*/, 0/*pChild*/, 0/*type*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setFrame(pCtx->pTarget, 356);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_addAnimationsStatic(pCtx->pTarget, _tgt_animData);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_playAnimation(pCtx->pTarget, 0);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Load and set its animations
    rv = gfmSprite_addAnimationsStatic(pCtx->pSpr, _pl_animData);
    ASSERT(rv == GFMRV_OK, rv);
    
    pCtx->curAnim = -1;
    rv = stPl_setAnimation(pCtx, SPR_ANIM_DEF);
    ASSERT(rv == GFMRV_OK, rv);
    
    rv = gfmSprite_setVerticalAcceleration(pCtx->pSpr, GAME_GRAV);
    ASSERT(rv == GFMRV_OK, rv);
    
    pCtx->vx = 50;
    pCtx->vy = 150;
    // TODO Tune this value
    pCtx->maxPropelSpeed = 75;
    
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
    gfmSprite_free(&((*ppCtx)->pTarget));
    
    free(*ppCtx);
    *ppCtx = 0;
}

/**
 * Collide the player against the wall/floor
 * 
 * @param  [in]pCtx  The player
 * @param  [in]pWall The wall
 * @return           GFMRV_OK, ...
 */
gfmRV pl_collideWall(player *pCtx, gameCtx *pGame, gfmObject *pWall) {
    gfmCollision dir;
    gfmObject *pObj;
    gfmRV rv;
    
    // Collide the player's object against the wall
    rv = gfmSprite_getObject(&pObj, pCtx->pSpr);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmObject_collide(pObj, pWall);
    ASSERT(rv == GFMRV_TRUE || rv == GFMRV_FALSE, rv);
    
    // If the player is on the floor, make it stand still
    rv = gfmSprite_getCurrentCollision(&dir, pCtx->pSpr);
    ASSERT(rv == GFMRV_OK, rv);
    if (dir & gfmCollision_down) {
        rv = gfmSprite_setVerticalVelocity(pCtx->pSpr, 0.0);
        ASSERT(rv == GFMRV_OK, rv);
    }
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Collide the player against a spike
 * 
 * @param  [in]pCtx   The player
 * @param  [in]pGame  The game context
 * @param  [in]pSpike The spike
 * @return            GFMRV_OK, ...
 */
gfmRV pl_collideSpike(player *pCtx, gameCtx *pGame, gfmObject *pSpike) {
    gfmCollision dir;
    gfmObject *pObj;
    gfmRV rv;
    
    // Collide the player's object against the wall
    rv = gfmSprite_getObject(&pObj, pCtx->pSpr);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmObject_collide(pObj, pSpike);
    ASSERT(rv == GFMRV_TRUE || rv == GFMRV_FALSE, rv);
    
    // If the player is on the floor, make it stand still
    rv = gfmSprite_getCurrentCollision(&dir, pCtx->pSpr);
    ASSERT(rv == GFMRV_OK, rv);
    if ((dir & gfmCollision_ver) != 0 && pCtx->respawnTimer <= 0) {
        // Kill the player
        pCtx->respawnTimer += PL_RESPAWN_TIME;
    }
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Collide the player against a stone of power
 * 
 * @param  [in]pCtx   The player
 * @param  [in]pGame  The game context
 * @param  [in]pStone The stone
 * @return            GFMRV_OK, ...
 */
gfmRV pl_collideStone(player *pCtx, gameCtx *pGame, stone *pStone) {
    gfmObject *pObj1, *pObj2;
    gfmRV rv;
    
    // Overlap the player's object against the stone
    rv = gfmSprite_getObject(&pObj1, pCtx->pSpr);
    ASSERT(rv == GFMRV_OK, rv);
    rv = st_getObject(&pObj2, pStone);
    ASSERT(rv == GFMRV_OK, rv);
    
    rv = gfmObject_isOverlaping(pObj1, pObj2);
    ASSERT(rv == GFMRV_TRUE || rv == GFMRV_FALSE, rv);
    
    // If it's overlaping, add its powers to the player
    if (rv == GFMRV_TRUE) {
        int type;
        
        rv = st_getType(&type, pStone);
        ASSERT(rv == GFMRV_OK, rv);
        
        rv = stPl_addPower(pCtx, type);
        ASSERT(rv == GFMRV_OK, rv);
        
        // Remove the stone from the screen
        rv = gfmObject_setPosition(pObj2, -320, -240);
        ASSERT(rv == GFMRV_OK, rv);
    }
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Update the player
 * 
 * @param  [in]pCtx The player
 * @return          GFMRV_OK, ...
 */
gfmRV pl_update(player *pPlayer, gameCtx *pGame) {
    gfmCamera *pCamera;
    gfmCollision dir;
    gfmCtx *pCtx;
    gfmInput *pInput;
    gfmRV rv;
    int elapsed, isShooting;
    
    pCtx = pGame->pCtx;
    rv = gfm_getElapsedTime(&elapsed, pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_getCamera(&pCamera, pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    isShooting = 0;
    
    /** == Pre-update (handle inputs) ======================================= */
    
    rv = gfmSprite_getCollision(&dir, pPlayer->pSpr);
    ASSERT(rv == GFMRV_OK, rv);
    
    if (pPlayer->respawnTimer > 0) {
        // Force the player to stand still
        rv = gfmSprite_setVelocity(pPlayer->pSpr, 0, 0);
        ASSERT(rv == GFMRV_OK, rv);
    }
    else if (dir & gfmCollision_down) {
        if (pGame->stRight & gfmInput_pressed) {
            rv = gfmSprite_setHorizontalVelocity(pPlayer->pSpr, pPlayer->vx);
            ASSERT(rv == GFMRV_OK, rv);
        }
        else if (pGame->stLeft & gfmInput_pressed) {
            rv = gfmSprite_setHorizontalVelocity(pPlayer->pSpr, -pPlayer->vx);
            ASSERT(rv == GFMRV_OK, rv);
        }
        else {
            rv = gfmSprite_setHorizontalVelocity(pPlayer->pSpr, 0/*vx*/);
            ASSERT(rv == GFMRV_OK, rv);
        }
    }
    
    // If the player is alive and touching the floor
    if (pPlayer->respawnTimer <= 0 && (dir & gfmCollision_down) &&
            (pGame->stJump & gfmInput_justPressed) == gfmInput_justPressed) {
        rv = gfmSprite_setVerticalVelocity(pPlayer->pSpr, -pPlayer->vy);
        ASSERT(rv == GFMRV_OK, rv);
    }
    
    if ((pGame->stShoot & gfmInput_justPressed) == gfmInput_justPressed &&
            pPlayer->curPower > 0) {
        // Remove a bar
        pPlayer->curPower--;
        // Set propeling speed
        pPlayer->propelSpeed = pPlayer->maxPropelSpeed;
        pPlayer->nextShoot = pPlayer->shootDelay;
        isShooting = 1;
    }
    else if ((pGame->stShoot & gfmInput_pressed) && (pPlayer->curPower > 0 ||
            pPlayer->nextShoot > 0)) {
        // Update the shooting time
        pPlayer->nextShoot -= elapsed;
        if (pPlayer->nextShoot <= 0 && pPlayer->curPower > 0) {
            pPlayer->curPower--;
            // Set propeling speed
            pPlayer->propelSpeed *= 0.9;
            pPlayer->nextShoot += pPlayer->shootDelay;
        }
        if (pPlayer->nextShoot > 0) {
            isShooting = 1;
        }
    }
    
    // Get the direction to which force will be applied
    rv = gfm_getInput(&pInput, pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmInput_getGamepadAnalog(&(pPlayer->dx), &(pPlayer->dy), pInput,
            0/*port*/, gfmController_rightAnalog);
    ASSERT(rv == GFMRV_OK, rv);
    // Apply a small deadzone of 0.3 x 0.3
    if (pPlayer->dx * pPlayer->dx + pPlayer->dy * pPlayer->dy < 0.09) {
        pPlayer->dx = 0;
        pPlayer->dy = 0;
    }
    
    // TODO Check if using the mouse and get from it
    
    if (isShooting) {
        // Propel the player
        rv = gfmSprite_setVelocity(pPlayer->pSpr,
                -pPlayer->dx * pPlayer->propelSpeed,
                -pPlayer->dy * pPlayer->propelSpeed);
        ASSERT(rv == GFMRV_OK, rv);
    }
    
    // If touching the floor, reset the shooting power
    if (dir & gfmCollision_down) {
        pPlayer->curPower = pPlayer->totalPower;
        pPlayer->nextShoot = 0;
    }
    
    /** == Update the physics =============================================== */
    rv = gfmSprite_update(pPlayer->pSpr, pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = collideSprite(pGame, pPlayer->pSpr);
    ASSERT(rv == GFMRV_OK, rv);
    
    if (pPlayer->respawnTimer > 0) {
        pPlayer->respawnTimer -= elapsed;
        if (pPlayer->respawnTimer <= 0) {
            int x, y;
            
            // Respawn the player
            rv = ps_getPlayerInitPos(&x, &y, pGame);
            ASSERT(rv == GFMRV_OK, rv);
            
            rv = gfmSprite_setPosition(pPlayer->pSpr, x, y - 16);
            ASSERT(rv == GFMRV_OK, rv);
        }
    }
    
    // Update the target, in case its animated
    rv = gfmSprite_update(pPlayer->pTarget, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    
    /** == Post-update (animation, move camera, etc) ======================== */
    
    // Check the player's direction
    do {
        double vx;
        
        rv = gfmSprite_getHorizontalVelocity(&vx, pPlayer->pSpr);
        ASSERT(rv == GFMRV_OK, rv);
        
        if (vx > 0) {
            rv = gfmSprite_setDirection(pPlayer->pSpr, 0/*isFlipped*/);
            ASSERT(rv == GFMRV_OK, rv);
        }
        else if (vx < 0) {
            rv = gfmSprite_setDirection(pPlayer->pSpr, 1/*isFlipped*/);
            ASSERT(rv == GFMRV_OK, rv);
        }
    } while (0);
    
    // Update the camera deadzone and position
    do {
        int cx, cy, isFlipped;
        int border, h, w, x, y;
        int alpha;
        
        border = 40;
        h = GAME_BBUF_HEIGHT - GAME_UI_HEIGHT - 160;
        w = 20;
        y = 80;
        
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
    
    // Update the current animation
    do {
        double vx, vy;
        
        rv = gfmSprite_getVelocity(&vx, &vy, pPlayer->pSpr);
        ASSERT(rv == GFMRV_OK, rv);
        
        rv = GFMRV_OK;
        if (pPlayer->respawnTimer > 0) {
            rv = stPl_setAnimation(pPlayer, SPR_ANIM_DEATH);
        }
        else if (isShooting) {
            rv = stPl_setAnimation(pPlayer, SPR_ANIM_LASER);
            
            // Check if should spawn particles
            if (pPlayer->lastParticle == 0 && pGame->maxParticles) {
                double ang, dang;
                gfmGroup  *pGrp;
                int curType, cx, cy, n, stones;
                
                // Retrieve the player's center
                rv = gfmSprite_getCenter(&cx, &cy, pPlayer->pSpr);
                ASSERT(rv == GFMRV_OK, rv);
                // Get the shooting angle
                ang = atan2((double)pPlayer->dx, (double)-pPlayer->dy);
                ang -= PI / 2.0;
                // Open the angle, according to how many bullet will be shot
                n = 0;
                curType = 1;
                while (curType < 0x0100) {
                    if (curType & pPlayer->stones)
                        n++;
                    curType <<= 1;
                }
                dang = PL_BUL_DANG * PI / 180.0;
                if (n > 0) {
                    ang -= dang * n / 2.0;
                }
                
                rv = ps_getParticles(&pGrp, pGame);
                ASSERT(rv == GFMRV_OK, rv);
                
                stones = pPlayer->stones;
                curType = 1;
                while (stones != 0) {
                    // If this color was gotten
                    if (stones & 1) {
                        double vx, vy;
                        gfmSprite *pSpr;
                        
                        vx = PL_BUL_SPEED*cos(ang);
                        vy = PL_BUL_SPEED*sin(ang);
                        
                        // Recycle the particle
                        rv = gfmGroup_recycle(&pSpr, pGrp);
                        ASSERT(rv == GFMRV_OK, rv);
                        rv = gfmGroup_setPosition(pGrp, cx, cy);
                        ASSERT(rv == GFMRV_OK, rv);
                        switch (curType) {
                            case RED_STONE:    rv = gfmGroup_setAnimation(pGrp, RED_BULLET); break;
                            case ORANGE_STONE: rv = gfmGroup_setAnimation(pGrp, ORANGE_BULLET); break;
                            case YELLOW_STONE: rv = gfmGroup_setAnimation(pGrp, YELLOW_BULLET); break;
                            case GREEN_STONE:  rv = gfmGroup_setAnimation(pGrp, GREEN_BULLET); break;
                            case CYAN_STONE:   rv = gfmGroup_setAnimation(pGrp, CYAN_BULLET); break;
                            case BLUE_STONE:   rv = gfmGroup_setAnimation(pGrp, BLUE_BULLET); break;
                            case PURPLE_STONE: rv = gfmGroup_setAnimation(pGrp, PURPLE_BULLET); break;
                            default: rv = GFMRV_FUNCTION_FAILED;
                        }
                        ASSERT(rv == GFMRV_OK, rv);
                        rv = gfmSprite_setVelocity(pSpr, vx, vy);
                        ASSERT(rv == GFMRV_OK, rv);
                        
                        ang += dang;
                    }
                    
                    stones >>= 1;
                    curType <<= 1;
                }
                
                pPlayer->lastParticle += pGame->particlesDelay;
            }
        }
        else if (vy != 0) {
            rv = stPl_setAnimation(pPlayer, SPR_ANIM_JUMP);
        }
        else if (vx != 0) {
            rv = stPl_setAnimation(pPlayer, SPR_ANIM_WALK);
            ASSERT(rv == GFMRV_OK, rv);
            if (vx > 0) {
                rv = gfmSprite_setDirection(pPlayer->pSpr, 0/*isFlipped*/);
            }
            else if (vx < 0) {
                rv = gfmSprite_setDirection(pPlayer->pSpr, 1/*isFlipped*/);
            }
        }
        else {
            rv = stPl_setAnimation(pPlayer, SPR_ANIM_DEF);
        }
        ASSERT(rv == GFMRV_OK, rv);
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
    gfmRV rv;
    
    rv = gfmSprite_draw(pPlayer->pSpr, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    
    if (pPlayer->dx != 0 || pPlayer->dy != 0) {
        int cx, cy;
        
        // Retrieve the player's current position
        rv = gfmSprite_getCenter(&cx, &cy, pPlayer->pSpr);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmSprite_setPosition(pPlayer->pTarget, cx + pPlayer->dx * 24,
                cy + pPlayer->dy * 24);
        ASSERT(rv == GFMRV_OK, rv);
        
        // Render it!
        rv = gfmSprite_draw(pPlayer->pTarget, pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
    }
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Draw the player's power bar
 * 
 * @param  [in]pCtx   The player
 * @param  [in]pGame  The game context
 * @param  [in]x      The horizontal position
 * @param  [in]y      The vertical position
 * @return            GFMRV_OK, ...
 */
gfmRV pl_drawPowerBar(player *pPlayer, gameCtx *pGame, int x, int y) {
    gfmCtx *pCtx;
    gfmSpriteset *pSset; 
    gfmRV rv;
    int i, tile;
    
    pCtx = pGame->pCtx;
    pSset = pGame->pSset2x8;
    
    tile = 1408;
    rv = gfm_drawTile(pCtx, pSset, x, y, tile, 0/*isFlipped*/);
    ASSERT(rv == GFMRV_OK, rv);
    
    x += 2;
    i = 0;
    while (i < pPlayer->curPower) {
        tile = 1409;
        tile += i >> 2;
        
        rv = gfm_drawTile(pCtx, pSset, x + 2*i, y, tile, 0/*isFlipped*/);
        ASSERT(rv == GFMRV_OK, rv);
        
        i++;
    }
    while (i < 7*4) {
        tile = 1416;
        tile += i >> 2;
        
        rv = gfm_drawTile(pCtx, pSset, x + 2*i, y, tile, 0/*isFlipped*/);
        ASSERT(rv == GFMRV_OK, rv);
        
        i++;
    }
    
    tile = 1423;
    rv = gfm_drawTile(pCtx, pSset, x + 2*i, y, tile, 0/*isFlipped*/);
    ASSERT(rv == GFMRV_OK, rv);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

