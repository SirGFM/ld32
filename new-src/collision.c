/**
 * @file new-src/collision.c
 * 
 * Check collisions and handle 'em accordingly
 */
#include <GFraMe/gfmObject.h>
#include <GFraMe/gfmSprite.h>

#include <ld32_pc/collision.h>
#include <ld32_pc/game.h>
#include <ld32_pc/player.h>

/**
 * Handle collision and updates it
 * 
 * @param [in]pGame The game context
 * @return          GFMRV_OK, ...
 */
static gfmRV collide(gameCtx *pGame) {
    gfmQuadtreeRoot *pQt;
    gfmRV rv;
    
    pQt = pGame->pQt;
    
    while (rv != GFMRV_QUADTREE_DONE) {
        gfmObject *pObj1, *pObj2;
        
        rv = gfmQuadtree_getOverlaping(&pObj1, &pObj2, pQt);
        ASSERT(rv == GFMRV_OK, rv);
        
        // TODO Retrieve both objects types
        
        rv = gfmQuadtree_continue(pQt);
        ASSERT(rv == GFMRV_QUADTREE_OVERLAPED || rv == GFMRV_QUADTREE_DONE, rv);
    }
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Collide a sprite against the world
 * 
 * @param [in]pGame The game context
 * @param [in]pSpr  The sprite
 * @return          GFMRV_OK, ...
 */
gfmRV collideSprite(gameCtx *pGame, gfmSprite *pSpr) {
    gfmRV rv;
    
    rv = gfmQuadtree_collideSprite(pGame->pQt, pSpr);
    ASSERT(rv == GFMRV_QUADTREE_OVERLAPED || rv == GFMRV_QUADTREE_DONE, rv);
    
    if (rv == GFMRV_QUADTREE_OVERLAPED) {
        rv = collide(pGame);
        ASSERT(rv == GFMRV_OK, rv);
    }
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

