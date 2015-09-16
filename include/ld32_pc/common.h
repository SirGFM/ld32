/**
 * @file include/ld33_pc/common.h
 * 
 * Common functionalities used through the states
 */
#ifndef __COMMON_H__
#define __COMMON_H__

#include <ld32_pc/game.h>

/**
 * Cache all pressed keys (and how many times they were pressed)
 * 
 * @param  [in]pGame The game's context
 * @return           GFMRV_OK, ...
 */
gfmRV common_cacheInput(gameCtx *pGame);

#if 0
/**
 * Set the game's PRNG's seed
 * 
 * @param  [in]pGame The game's context
 * @param  [in]seed  The seed
 */
void common_setPRNGSeed(gameCtx *pGame, int seed);

/**
 * Updates the game's seed and return a pseudo-random number
 * 
 * @param  [in]pGame The game's context
 * @return           The pseudo-random number
 */
int common_getPRN(gameCtx *pGame);
#endif /* 0 */

#endif /* __COMMON_H__ */

