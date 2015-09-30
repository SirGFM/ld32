/**
 * @file src/common.c
 * 
 * Common functionalities used through the states
 */
#include <GFraMe/gframe.h>
#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>

#include <ld32_pc/common.h>

/**
 * Cache all pressed keys (and how many times they were pressed)
 * 
 * @param  [in]pGame The game's context
 * @return           GFMRV_OK, ...
 */
gfmRV common_cacheInput(gameCtx *pGame) {
    gfmRV rv;
    
    // Macro to easily retrieve every keys' state
#define GET_KEY_STATE(key) \
    rv = gfm_getKeyState(&(pGame->st##key), &(pGame->n##key), \
        pGame->pCtx, pGame->hnd##key); \
    ASSERT(rv == GFMRV_OK, rv)
    
    // Retrieve the keys' state
    GET_KEY_STATE(Left);
    GET_KEY_STATE(Right);
    GET_KEY_STATE(Jump);
    GET_KEY_STATE(Shoot);
    GET_KEY_STATE(Fullscreen);
    GET_KEY_STATE(Mute);
    GET_KEY_STATE(VolUp);
    GET_KEY_STATE(VolDown);
    GET_KEY_STATE(Quit);
    GET_KEY_STATE(Gif);
    
#undef GET_KEY_STATE
    
    // If the quit key was pressed, set the quit flag
    if ((pGame->stQuit & gfmInput_justPressed) == gfmInput_justPressed) {
        rv = gfm_setQuitFlag(pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
    }
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Set the game's PRNG's seed
 * 
 * @param  [in]pGame The game's context
 * @param  [in]seed  The seed
 */
void common_setPRNGSeed(gameCtx *pGame, int seed) {
    pGame->seed = seed;
}

/**
 * Updates the game's seed and return a pseudo-random number
 * 
 * @param  [in]pGame The game's context
 * @return           The pseudo-random number
 */
int common_getPRN(gameCtx *pGame) {
    long int tmp = pGame->seed;
    
    tmp *= 0x19660d;
    tmp += 0x3c6ef35f;
    pGame->seed = tmp;
    
    return pGame->seed;
}

