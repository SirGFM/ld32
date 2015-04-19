/**
 * @file src/collision.h
 * 
 * Collision handler
 */

#include "collision.h"
#include "player.h"
#include "sprite.h"

#ifdef DEBUG
#include <stdio.h>
#endif

/**
 * Collide two sprites
 */
void collisionCallback(sprite *pSpr1, sprite *pSpr2, sprType type1,
        sprType type2) {
    if (type1 == SPR_PLAYER)
        collPlSpr(pSpr1, pSpr2, type2);
    else if (type2 == SPR_PLAYER)
        collPlSpr(pSpr2, pSpr1, type1);
    else {
#ifdef DEBUG
        printf("Unkown collision!!\n");
#endif
    }
}

/**
 * Collide a sprite against a player
 */
void collPlSpr(sprite *pSprPl, sprite *pSpr, sprType type) {
    player *pPl;
    
    spr_getSuper((void**)&pPl, pSprPl);
    
    switch (type) {
        case SPR_RED_STONE:
        case SPR_ORANGE_STONE:
        case SPR_YELLOW_STONE:
        case SPR_GREEN_STONE:
        case SPR_CYAN_STONE:
        case SPR_BLUE_STONE:
        case SPR_PURPLE_STONE:
            pl_addStone(pPl, type);
            spr_kill(pSpr);
        break;
        default: {}
    }
}
