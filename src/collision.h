/**
 * @file src/collision.h
 * 
 * Collision handler
 */
#ifndef __COLLISION_H__
#define __COLLISION_H__

#include "sprite.h"

/**
 * Collide two sprites
 */
void collisionCallback(sprite *pSpr1, sprite *pSpr2, sprType type1,
        sprType type2);

/**
 * Collide a sprite against a player
 */
void collPlSpr(sprite *pSprPl, sprite *pSpr, sprType type);

#endif /* __COLLISION_H__ */

