/**
 * @file new-src/collision.c
 * 
 * Check collisions and handle 'em accordingly
 */
#include <GFraMe/gfmGroup.h>
#include <GFraMe/gfmObject.h>
#include <GFraMe/gfmSprite.h>

#include <ld32_pc/collision.h>
#include <ld32_pc/game.h>
#include <ld32_pc/player.h>
#include <ld32_pc/playstate.h>

/**
 * Collide a particle against a wall
 */
static gfmRV cl_collideParticle(gfmSprite *pParticle, gameCtx *pGame,
        gfmObject *pWall) {
    gfmRV rv;
    int index;
    
    rv =  gfmSprite_getAnimationIndex(&index, pParticle);
    ASSERT(rv == GFMRV_OK, rv);
    
    if (index < RED_EXPLOSION) {
        index += RED_EXPLOSION - RED_BULLET;
        rv = gfmSprite_playAnimation(pParticle, index);
        ASSERT(rv == GFMRV_OK, rv);
    }
    else {
        rv = gfmSprite_didAnimationFinish(pParticle);
        if (rv == GFMRV_TRUE) {
            gfmGroupNode *pNode;
            int type;
            
            rv = gfmSprite_getChild((void**)&pNode, &type, pParticle);
            ASSERT(rv == GFMRV_OK, rv);
            
            rv = gfmGroup_removeNode(pNode);
            ASSERT(rv == GFMRV_OK, rv);
        }
    }
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

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
    
    rv = GFMRV_QUADTREE_OVERLAPED;
    while (rv != GFMRV_QUADTREE_DONE) {
        gfmObject *pObj1, *pObj2;
        gfmSprite *pSpr1, *pSpr2;
        int type1, type2;
        void *pChild1, *pChild2;
        
        rv = gfmQuadtree_getOverlaping(&pObj1, &pObj2, pQt);
        ASSERT(rv == GFMRV_OK, rv);
        
        // Retrieve both objects types
        rv = gfmObject_getChild((void**)&pSpr1, &type1, pObj1);
        ASSERT(rv == GFMRV_OK, rv);
        if (type1 == gfmType_sprite) {
            rv = gfmSprite_getChild(&pChild1, &type1, pSpr1);
            ASSERT(rv == GFMRV_OK, rv);
        }
        else {
            pChild1 = pObj1;
        }
        if (type1 == tParticle) {
            pChild1 = pSpr1;
        }
        
        rv = gfmObject_getChild((void**)&pSpr2, &type2, pObj2);
        ASSERT(rv == GFMRV_OK, rv);
        if (type2 == gfmType_sprite) {
            rv = gfmSprite_getChild(&pChild2, &type2, pSpr2);
            ASSERT(rv == GFMRV_OK, rv);
        }
        else {
            pChild2 = pObj2;
        }
        if (type2 == tParticle) {
            pChild2 = pSpr2;
        }
        
#define HANDLE_COLLISION(TYPEA, TYPEB, COL_FUNC) \
        else if (type1 == TYPEA && type2 == TYPEB) { rv = COL_FUNC(pChild1, pGame, pChild2); } \
        else if (type2 == TYPEA && type1 == TYPEB) { rv = COL_FUNC(pChild2, pGame, pChild1); }
#define IGNORE_COLLISION(TYPEA, TYPEB) \
        else if (type1 == TYPEA && type2 == TYPEB) { rv = GFMRV_OK; } \
        else if (type2 == TYPEA && type1 == TYPEB) { rv = GFMRV_OK; }
        
        if (0) {}
        HANDLE_COLLISION(tPlayer, tCollideable, pl_collideWall)
        HANDLE_COLLISION(tPlayer, tSpike, pl_collideSpike)
        HANDLE_COLLISION(tPlayer, tCheckpoint, ps_plCollideCheckpoint)
        HANDLE_COLLISION(tPlayer, tPowerstone, pl_collideStone)
        HANDLE_COLLISION(tParticle, tCollideable, cl_collideParticle)
        IGNORE_COLLISION(tPowerstone, tCheckpoint)
        IGNORE_COLLISION(tPowerstone, tSpike)
        IGNORE_COLLISION(tPowerstone, tCollideable)
        IGNORE_COLLISION(tParticle, tCheckpoint)
        IGNORE_COLLISION(tParticle, tPlayer)
        IGNORE_COLLISION(tParticle, tParticle)
        IGNORE_COLLISION(tParticle, tPowerstone)
        IGNORE_COLLISION(tParticle, tSpike)
        IGNORE_COLLISION(tParticle, tParticle)
        else { rv = GFMRV_FUNCTION_FAILED; }
        
        ASSERT(rv == GFMRV_OK, rv);
        
#undef IGNORE_COLLISION
#undef HANDLE_COLLISION
        
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

/**
 * Collide a group against the world
 * 
 * @param  [in]pGame The game context
 * @param  [in]pGrp  The group
 * @return           GFMRV_OK, ...
 */
gfmRV collideGroup(gameCtx *pGame, gfmGroup *pGrp) {
    gfmRV rv;
    
    rv = gfmQuadtree_collideGroup(pGame->pQt, pGrp);
    ASSERT(rv == GFMRV_QUADTREE_OVERLAPED || rv == GFMRV_QUADTREE_DONE, rv);
    
    if (rv == GFMRV_QUADTREE_OVERLAPED) {
        rv = collide(pGame);
        ASSERT(rv == GFMRV_OK, rv);
    }
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

