/**
 * @file new-src/main.c
 * 
 * The game's entry point
 */
#include <ld32_pc/common.h>
#include <ld32_pc/game.h>
#include <ld32_pc/playstate.h>

#if defined(EMSCRIPT)
#  include <emscripten.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(EMSCRIPT)
static void main_loop(void *pArg) {
#else
static gfmRV main_loop(void *pArg) {
#endif
    gameCtx *pGame;
    gfmRV rv;
    
    // Retrieve the game's context
    pGame = (gameCtx*)pArg;
    
    // Initialize the game (only once)
    if (pGame->firstFrame == 0) {
        rv = ps_init(pGame);
        ASSERT(rv == GFMRV_OK, rv);
        
        pGame->firstFrame = 1;
    }
    
#if defined(EMSCRIPT)
    // If on HTML5, must issue a frame
    rv = gfm_issueFrame(pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
#else
    while (gfm_didGetQuitFlag(pGame->pCtx) == GFMRV_FALSE) {
#endif
        // Update the events
        rv = gfm_handleEvents(pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
        
        // Update the game as many times as necessary
        while (gfm_isUpdating(pGame->pCtx) == GFMRV_TRUE) {
            // Start the FPS counter (on debug mode only)
            rv = gfm_fpsCounterUpdateBegin(pGame->pCtx);
            ASSERT(rv == GFMRV_OK, rv);
            
            // Cache the inputs
            rv =  common_cacheInput(pGame);
            ASSERT(rv == GFMRV_OK, rv);
            
            // Call the state's update
            rv = ps_update(pGame);
            ASSERT(rv == GFMRV_OK, rv);
            
            // Calculate how long this frame took (debug mode only)
            rv = gfm_fpsCounterUpdateEnd(pGame->pCtx);
            ASSERT(rv == GFMRV_OK, rv);
        }
        
        // Draw the game, if a frame was issued
        while (gfm_isDrawing(pGame->pCtx) == GFMRV_TRUE) {
            // Clear the backbuffer and set it as the render target
            rv = gfm_drawBegin(pGame->pCtx);
            ASSERT(rv == GFMRV_OK, rv);
            
            // Call the state's draw
            rv = ps_draw(pGame);
            ASSERT(rv == GFMRV_OK, rv);
            
            // Render the backbuffer to the screen and flip it
            rv = gfm_drawEnd(pGame->pCtx);
            ASSERT(rv == GFMRV_OK, rv);
        }
        
#if !defined(EMSCRIPT)
    }
#else
    // If on HTML5, sleep until the next frame
    rv = gfm_waitFrame(pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
#endif
    
#if !defined(EMSCRIPT)
    // Clean the state, if the game exited
    ps_clean(pGame);
#endif

__ret:
#if defined(EMSCRIPT)
    if (rv != GFMRV_OK) {
        printf("Got error %i\n", rv);
    }
#else
    return rv;
#endif
}

/**
 * Load all assets; It should run on another thread while the main plays an
 * animations, but I didn't get to implement that... yet... XD
 * 
 * @param  [in]pGame The game
 * @return           ...
 */
static gfmRV loadAssets(gameCtx *pGame) {
    gfmRV rv;
    int texIndex;
    
    // Load the texture and set it as default (since it will be the only one)
    rv = gfm_loadTextureStatic(&texIndex, pGame->pCtx, "atlas.bmp",
            0xff00ff/*keyColor*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_setDefaultTexture(pGame->pCtx, texIndex);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Create the texture's spritesets
    rv = gfm_createSpritesetCached(&(pGame->pSset2x8), pGame->pCtx, texIndex,
        2/*tileWidth*/, 8/*tileHeight*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_createSpritesetCached(&(pGame->pSset8x8), pGame->pCtx, texIndex,
        8/*tileWidth*/, 8/*tileHeight*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_createSpritesetCached(&(pGame->pSset16x16), pGame->pCtx, texIndex,
        16/*tileWidth*/, 16/*tileHeight*/);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Load the song and set it to loop
#if 0
    rv = gfm_loadAudio(&(pGame->song), pGame->pCtx, "mysong.wav", 10);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_setRepeat(pGame->pCtx, pGame->song, 6 * pGame->audioFreq );
    ASSERT(rv == GFMRV_OK, rv);
    
    // Load all SFXs
    rv = gfm_loadAudio(&(pGame->expl), pGame->pCtx, "expl.wav", 8);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_loadAudio(&(pGame->wall_hit), pGame->pCtx, "wall_hit.wav", 12);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_loadAudio(&(pGame->slime_death), pGame->pCtx, "slime_death.wav", 15);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_loadAudio(&(pGame->slime_hit), pGame->pCtx, "slime_hit.wav", 13);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_loadAudio(&(pGame->pl_hit), pGame->pCtx, "player_hit.wav", 14);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_loadAudio(&(pGame->pl_death), pGame->pCtx, "player_death.wav", 16);
    ASSERT(rv == GFMRV_OK, rv);
#endif
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Game's entry point; On Desktop, it call main_loop and stays there until the
 * game is closed; On HTML5, it sets main_loop as a callback and exits main
 * 
 * @param  [in]argc Number of arguments in argv
 * @param  [in]argv Arguments passed on the command-line
 */
int main(int argc, char *argv[]) {
    gameCtx *pGame;
    gfmAudioQuality audSettings;
    gfmInput *pInput;
    gfmRV rv;
    int bbufWidth, bbufHeight, dps, fps, height, isFullscreen, ups,
            width;
    
    // Set default values
    pGame = 0;
    
    // Alloc the game's context
    pGame = (gameCtx*)malloc(sizeof(gameCtx));
    ASSERT(pGame, GFMRV_ALLOC_FAILED);
    // Clean everything before hand
    memset(pGame, 0x0, sizeof(gameCtx));
    
    // Start the library
    rv = gfm_getNew(&(pGame->pCtx));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_initStatic(pGame->pCtx, "com.gfmgamecorner", "ld32-v2-port");
    ASSERT(rv == GFMRV_OK, rv);
    
    // Intialize the seed
    //common_setPRNGSeed(pGame, (unsigned int)time(0));
    
    // Set default options
    isFullscreen = 0;
    width = 640;
    height = 480;
    pGame->deadzoneSpeed = 4;
    pGame->maxParticles = 4096;
    //pGame->maxParts = 2048;
    //pGame->audioFreq = 44100;
    audSettings = gfmAudio_defQuality;
    
#if !defined(EMSCRIPT)
    // 'Parse' options
    while (argc > 1) {
        #define GETARG(opt) strcmp(argv[argc - 1], opt) == 0
        
        if (GETARG("-full") || GETARG("-f")) {
            isFullscreen = 1;
        }
        else if (GETARG("-noaudio") || GETARG("-m")) {
            rv =  gfm_disableAudio(pGame->pCtx);
            ASSERT(rv == GFMRV_OK, rv);
        }
        else if (GETARG("-badaudio") || GETARG("-l")) {
            // TODO Test with lowQuality
            audSettings = gfmAudio_medQuality;
            //pGame->audioFreq = 22050;
        }
        else if (GETARG("-width") || GETARG("-w")) {
            char *pTmp;
            
            width = 0;
            pTmp = argv[argc];
            while (*pTmp) {
                width = width * 10 + (*pTmp) - '0';
                
                pTmp++;
            }
        }
        else if (GETARG("-height") || GETARG("-h")) {
            char *pTmp;
            
            height = 0;
            pTmp = argv[argc];
            while (*pTmp) {
                height = height * 10 + (*pTmp) - '0';
                
                pTmp++;
            }
        }
        
        #undef GETARG
        argc--;
    }
#endif
    
    // Create the window
    bbufWidth = GAME_BBUF_WIDTH;
    bbufHeight = GAME_BBUF_HEIGHT;
    if (isFullscreen) {
        rv = gfm_initGameFullScreen(pGame->pCtx, bbufWidth, bbufHeight,
                0/*defRes*/, 0/*dontResize*/);
    }
    else {
        rv = gfm_initGameWindow(pGame->pCtx, bbufWidth, bbufHeight, width, height,
                0/*dontResize*/);
    }
    ASSERT(rv == GFMRV_OK, rv);
    
    // Set the BG color
    rv = gfm_setBackground(pGame->pCtx, 0xff5b6ee1);
    ASSERT(rv == GFMRV_OK, rv);
    
    // TODO Remove this
    rv = gfm_disableAudio(pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    // Initialize the audio system
    rv = gfm_initAudio(pGame->pCtx, audSettings);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Shorten the double press window
    rv = gfm_getInput(&pInput, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmInput_setMultiDelay(pInput, 200/*ms*/);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Bind keys
#define BIND_NEW_KEY(handle, key) \
    rv = gfm_addVirtualKey(&(pGame->hnd##handle), pGame->pCtx); \
    ASSERT(rv == GFMRV_OK, rv); \
    rv = gfm_bindInput(pGame->pCtx, pGame->hnd##handle, key); \
    ASSERT(rv == GFMRV_OK, rv)
#define BIND_KEY(handle, key) \
    rv = gfm_bindInput(pGame->pCtx, pGame->hnd##handle, key); \
    ASSERT(rv == GFMRV_OK, rv)
#define BIND_BUTTON(handle, button) \
    rv = gfm_bindGamepadInput(pGame->pCtx, pGame->hnd##handle, button, 0); \
    ASSERT(rv == GFMRV_OK, rv)
    
    // TODO Bind default gamepad buttons
    BIND_NEW_KEY(Left, gfmKey_left);
    BIND_KEY(Left, gfmKey_a);
    BIND_BUTTON(Left, gfmController_left);
    BIND_BUTTON(Left, gfmController_laxis_left);
    BIND_NEW_KEY(Right, gfmKey_right);
    BIND_KEY(Right, gfmKey_d);
    BIND_BUTTON(Right, gfmController_right);
    BIND_BUTTON(Right, gfmController_laxis_right);
    BIND_NEW_KEY(Jump, gfmKey_up);
    BIND_KEY(Jump, gfmKey_w);
    BIND_BUTTON(Jump, gfmController_l1);
    BIND_BUTTON(Jump, gfmController_r1);
    BIND_NEW_KEY(Shoot, gfmKey_x);
    BIND_KEY(Shoot, gfmKey_space);
    BIND_BUTTON(Shoot, gfmController_l2);
    BIND_BUTTON(Shoot, gfmController_r2);
    BIND_NEW_KEY(Quit, gfmKey_esc);
    BIND_NEW_KEY(Fullscreen, gfmKey_f12);
    
#undef BIND_NEW_KEY
#undef BIND_KEY
    
    // Load assets
    rv = loadAssets(pGame);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Set FPS
    ups = 60;
    dps = 60;
    rv = gfm_setStateFrameRate(pGame->pCtx, ups, dps);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Set the timer resolution, in frames per seconds
    fps = 60;
    rv = gfm_setFPS(pGame->pCtx, fps);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Initialize the FPS counter (only visible in debug mode, though)
    rv = gfm_initFPSCounter(pGame->pCtx, pGame->pSset8x8, 0/*firstTile*/);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Initialize the quadtree
    rv = gfmQuadtree_getNew(&(pGame->pQt));
    ASSERT(rv == GFMRV_OK, rv);
    
    // Play the song
#if 0
    rv = gfm_playAudio(0, pGame->pCtx, pGame->song, 0.8);
    ASSERT(rv == GFMRV_OK, rv);
#endif
    
    // Set this flag, so the state is initialized on the first iteration
    pGame->firstFrame = 0;
    
#if defined(EMSCRIPT)
    // Set the game's callback and exit main
    emscripten_set_main_loop_arg((em_arg_callback_func)main_loop, pGame, 0, 0);
    rv = GFMRV_OK;
#else
    // Run the game in an infinite loop
    rv = main_loop((void*)pGame);
#endif
    
__ret:
#if !defined(EMSCRIPT)
    // Clean all resources (Desktop only)
    if (pGame) {
        //gfmGroup_free(&(pGame->pRender));
        //gfmGenArr_clean(pGame->pObjs, gfmObject_free);
        gfmQuadtree_free(&(pGame->pQt));
        gfm_free(&(pGame->pCtx));
        // Clear the actual game context
        free(pGame);
    }
#endif
    return rv;
}

