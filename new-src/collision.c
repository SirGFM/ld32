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
 * @param  [in]pGame The game context
 * @return           GFMRV_OK, ...
 */
static gfmRV collide(gameCtx *pGame) {
    gfmQuadtreeRoot *pQt;
    gfmRV rv;
    
    pQt = pGame->pQt;
    
    while (rv != GFMRV_QUADTREE_DONE) {
        gfmObject *pObj1, *pObj2;
        int type1, type2;
        void *pChild1, *pChild2;
        
        rv = gfmQuadtree_getOverlaping(&pObj1, &pObj2, pQt);
        ASSERT(rv == GFMRV_OK, rv);
        
        // Retrieve both objects types
        rv = gfmObject_getChild(&pChild1, &type1, pObj1);
        ASSERT(rv == GFMRV_OK, rv);
        if (type1 == gfmType_sprite) {
            rv = gfmSprite_getChild(&pChild1, &type1, (gfmSprite*)pChild1);
            ASSERT(rv == GFMRV_OK, rv);
        }
        else {
            pChild1 = pObj1;
        }
        
        rv = gfmObject_getChild(&pChild2, &type2, pObj2);
        ASSERT(rv == GFMRV_OK, rv);
        if (type2 == gfmType_sprite) {
            rv = gfmSprite_getChild(&pChild2, &type2, (gfmSprite*)pChild2);
            ASSERT(rv == GFMRV_OK, rv);
        }
        else {
            pChild2 = pObj2;
        }
        
#define HANDLE_COLLISION(TYPEA, TYPEB, COL_FUNC) \
        else if (type1 == TYPEA && type2 == TYPEB) { rv = COL_FUNC(pChild1, pGame, pChild2); } \
        else if (type2 == TYPEA && type1 == TYPEB) { rv = COL_FUNC(pChild2, pGame, pChild1); }
        
        if (0) {}
        HANDLE_COLLISION(tPlayer, tCollideable, pl_collideWall)
        else { rv = GFMRV_FUNCTION_FAILED; }
        
        ASSERT(rv == GFMRV_OK, rv);
        
#undef HANDLE_COL
        
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
 * @param  [in]pGame The game context
 * @param  [in]pSpr  The sprite
 * @return           GFMRV_OK, ...
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

