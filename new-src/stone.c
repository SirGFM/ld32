/**
 * @file new-src/stone.c
 * 
 * Power stones that gives the player a new color to shoot
 */
#include <GFraMe/gframe.h>
#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmParser.h>

#include <ld32_pc/collision.h>
#include <ld32_pc/game.h>
#include <ld32_pc/stone.h>

#include <stdlib.h>
#include <string.h>

enum {
    RED_STONE_ANIM,
    ORANGE_STONE_ANIM,
    YELLOW_STONE_ANIM,
    GREEN_STONE_ANIM,
    CYAN_STONE_ANIM,
    BLUE_STONE_ANIM,
    PURPLE_STONE_ANIM
};

static int _st_animData[] = 
{
/*                 */ /*len|fps|loop|frames...*/
/* RED_STONE_ANIM  */    8 , 12,  1 , 369,369,369,369,369,330,337,362,
/*ORANGE_STONE_ANIM*/    8 , 12,  1 , 370,370,370,370,370,331,338,363,
/*YELLOW_STONE_ANIM*/    8 , 12,  1 , 371,371,371,371,371,332,339,364,
/*GREEN_STONE_ANIM */    8 , 12,  1 , 372,372,372,372,372,333,340,365,
/* CYAN_STONE_ANIM */    8 , 12,  1 , 373,373,373,373,373,334,341,366,
/* BLUE_STONE_ANIM */    8 , 12,  1 , 374,374,374,374,374,335,342,367,
/*PURPLE_STONE_ANIM*/    8 , 12,  1 , 375,375,375,375,375,336,343,368,
};

struct stStone {
    gfmSprite *pSpr;
    int iniY;
    int delta;
    int type;
};

/**
 * Initialize a stone
 * 
 * @param  [out]ppCtx   The stone
 * @param  [in] pGame   The game context (to retrieve the texture and spriteset)
 * @param  [in] pParser The parser context (to retrieve the player's attributes)
 * @return              GFMRV_OK, GFMRV_ALLOC_FAILED, ...
 */
gfmRV st_init(stone **ppCtx, gameCtx *pGame, gfmParser *pParser) {
    gfmRV rv;
    stone *pCtx;
    int anim, color, delta, num, x, y;
    
    // Set these to avoid errors
    pCtx = 0;
    anim = -1;
    color = -1;
    delta = 0;
    
    // Retrieve the stone's attributes from the parser
    rv = gfmParser_getPos(&x, &y, pParser);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Retrieve all properties
    rv = gfmParser_getNumProperties(&num, pParser);
    ASSERT(rv == GFMRV_OK, rv);
    ASSERT(num > 0, GFMRV_FUNCTION_FAILED);
    while (num > 0) {
        char *pKey, *pVal;
        
        // Decrease early, as it starts as the length (i.e., last index+1)
        num--;
        rv = gfmParser_getProperty(&pKey, &pVal, pParser, num);
        ASSERT(rv == GFMRV_OK, rv);
        
        if (strcmp(pKey, "color") == 0) {
            if (0) {}
            #define SET_COLOR(colorStr, colorConst) \
                else if (strcmp(pVal, colorStr) == 0) { \
                    color = colorConst##_STONE; anim = colorConst##_STONE_ANIM; }
            SET_COLOR("red"   , RED)
            SET_COLOR("orange", ORANGE)
            SET_COLOR("yellow", YELLOW)
            SET_COLOR("green" , GREEN)
            SET_COLOR("cyan"  , CYAN)
            SET_COLOR("blue"  , BLUE)
            SET_COLOR("purple", PURPLE)
            #undef SET_COLOR
            else {
                ASSERT(0, GFMRV_FUNCTION_FAILED);
            }
        }
        else if (strcmp(pKey, "delta") == 0) {
            delta = 0;
            while (*pVal != '\0') {
                delta = delta * 10 + (*pVal) - '0';
                pVal++;
            }
        }
    }
    ASSERT(color != -1, GFMRV_FUNCTION_FAILED);
    
    // Alloc the stone
    pCtx = (stone*)malloc(sizeof(stone));
    ASSERT(pCtx, GFMRV_ALLOC_FAILED);
    memset(pCtx, 0x0, sizeof(stone));
    
    // Initialize its sprite
    y -= 8;
    rv = gfmSprite_getNew(&(pCtx->pSpr));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_init(pCtx->pSpr, x, y, 10/*width*/, 10/*height*/,
            pGame->pSset8x8, 1/*offX*/, 1/*offY*/, pCtx/*pChild*/,
            tPowerstone/*type*/);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Load and set its animations
    rv = gfmSprite_addAnimationsStatic(pCtx->pSpr, _st_animData);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_playAnimation(pCtx->pSpr, anim);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Set the stone's initial acceleration
    rv = gfmSprite_setVerticalAcceleration(pCtx->pSpr, -25);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Set the last attributes
    pCtx->iniY = y;
    pCtx->delta = delta;
    pCtx->type = color;
    
    *ppCtx = pCtx;
    pCtx = 0;
    rv = GFMRV_OK;
__ret:
    if (pCtx) {
        st_clean(&pCtx);
    }
    
    return rv;
}

/**
 * Free all memory used by the stone
 * 
 * @param  [in|out]ppCtx The stone
 */
void st_clean(stone **ppCtx) {
    if (!ppCtx || !(*ppCtx))
        return;
    
    gfmSprite_free(&((*ppCtx)->pSpr));
    
    free(*ppCtx);
    *ppCtx = 0;
}

/**
 * Retrieve the stone's type
 * 
 * @param  [out]pType  The type
 * @param  [in] pStone The stone
 * @return             GFMRV_OK, ...
 */
gfmRV st_getType(int *pType, stone *pStone) {
    *pType = pStone->type;
    return GFMRV_OK;
}

/**
 * Retrieve the stone's object
 * 
 * @param  [out]ppObj  The retrieved object
 * @param  [in] pStone The stone
 * @return             GFMRV_OK, ...
 */
gfmRV st_getObject(gfmObject **ppObj, stone *pStone) {
    return gfmSprite_getObject(ppObj, pStone->pSpr);
}

/**
 * Update the stone
 * 
 * @param  [in]pCtx The stone
 * @return          GFMRV_OK, ...
 */
gfmRV st_update(stone *pStone, gameCtx *pGame) {
    gfmCamera *pCam;
    gfmCtx *pCtx;
    gfmRV rv;
    int y;
    
    pCtx = pGame->pCtx;
    
    // Check the stone's position
    rv = gfmSprite_getVerticalPosition(&y, pStone->pSpr);
    ASSERT(rv == GFMRV_OK, rv);
    
    if (y > pStone->iniY + pStone->delta) {
        rv = gfmSprite_setVerticalAcceleration(pStone->pSpr, -25);
        ASSERT(rv == GFMRV_OK, rv);
    }
    else if (y < pStone->iniY - pStone->delta) {
        rv = gfmSprite_setVerticalAcceleration(pStone->pSpr, 25);
        ASSERT(rv == GFMRV_OK, rv);
    }
    
    // Cap the speed to avoid problems...
    do {
        double vy;
        
        rv = gfmSprite_getVerticalVelocity(&vy, pStone->pSpr);
        ASSERT(rv == GFMRV_OK, rv);
        
        if (vy > 10) {
            rv = gfmSprite_setVerticalVelocity(pStone->pSpr, 10);
            ASSERT(rv == GFMRV_OK, rv);
        }
        else if (vy < -10) {
            rv = gfmSprite_setVerticalVelocity(pStone->pSpr, -10);
            ASSERT(rv == GFMRV_OK, rv);
        }
    } while (0);
    
    // Actually update the sprite
    rv = gfmSprite_update(pStone->pSpr, pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Spawn particles, if on screen
    rv = gfm_getCamera(&pCam, pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmCamera_isSpriteInside(pCam, pStone->pSpr);
    ASSERT(rv == GFMRV_FALSE || rv == GFMRV_TRUE, rv);
    if (rv == GFMRV_TRUE) {
        // TODO Spawn particles, if on screen
    }
    
    // Add it to the qt
    rv = collideSprite(pGame, pStone->pSpr);
    ASSERT(rv == GFMRV_OK, rv);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Render the stone
 * 
 * @param  [in]pCtx The stone
 * @return          GFMRV_OK, ...
 */
gfmRV st_draw(stone *pStone, gameCtx *pGame) {
    return gfmSprite_draw(pStone->pSpr, pGame->pCtx);
}

