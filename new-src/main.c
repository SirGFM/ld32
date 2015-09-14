/**
 * @file src/main.c
 * 
 * The game's entry point
 */
#include <ld32_pc/game.h>

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
            ps_update(pGame);
            
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
            ps_draw(pGame);
            
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
 * @param  pGame  The game
 * @return        ...
 */
static gfmRV loadAssets(gameCtx *pGame) {
    gfmRV rv;
    int texIndex;
    
    // Load the texture and set it as default (since it will be the only one)
    rv = gfm_loadTextureStatic(&texIndex, pGame->pCtx, "new-atlas.bmp",
            0xff00ff/*keyColor*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_setDefaultTexture(pGame->pCtx, texIndex);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Create the texture's spritesets
    rv = gfm_createSpritesetCached(&(pGame->pSset4x4), pGame->pCtx, texIndex,
        4/*tileWidth*/, 4/*tileHeight*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_createSpritesetCached(&(pGame->pSset8x8), pGame->pCtx, texIndex,
        8/*tileWidth*/, 8/*tileHeight*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_createSpritesetCached(&(pGame->pSset32x32), pGame->pCtx, texIndex,
        32/*tileWidth*/, 32/*tileHeight*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_createSpritesetCached(&(pGame->pSset256x128), pGame->pCtx,
        texIndex, 256/*tileWidth*/, 128/*tileHeight*/);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Load the song and set it to loop
#if !defined(EMSCRIPT)
    rv = gfm_loadAudio(&(pGame->song), pGame->pCtx, "mysong.wav", 10);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_setRepeat(pGame->pCtx, pGame->song, 6 * pGame->audioFreq );
    ASSERT(rv == GFMRV_OK, rv);
#endif
    
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
    int bbufWidth, bbufHeight, doSkip, dps, fps, height, isFullscreen, ups,
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
    rv = gfm_initStatic(pGame->pCtx, "com.gfmgamecorner", "HerosQuest-post_compo");
    ASSERT(rv == GFMRV_OK, rv);
    
    // Intialize the seed
    common_setPRNGSeed(pGame, (unsigned int)time(0));
    
    // Set default options
    isFullscreen = 0;
    width = 640;
    height = 480;
    pGame->maxParts = 2048;
    pGame->audioFreq = 44100;
    audSettings = gfmAudio_defQuality;
    doSkip = 0;
    
#if !defined(EMSCRIPT)
    // 'Parse' options
    while (argc > 1) {
        #define GETARG(opt) strcmp(argv[argc - 1], opt) == 0
        
        if (GETARG("-full") || GETARG("-f")) {
            isFullscreen = 1;
        }
        else if (GETARG("-skip") || GETARG("-s")) {
            doSkip = 1;
        }
        else if (GETARG("-noaudio") || GETARG("-m")) {
            rv =  gfm_disableAudio(pGame->pCtx);
            ASSERT(rv == GFMRV_OK, rv);
        }
        else if (GETARG("-badaudio") || GETARG("-l")) {
            // TODO Test with lowQuality
            audSettings = gfmAudio_medQuality;
            pGame->audioFreq = 22050;
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
    bbufWidth = 320;
    bbufHeight = 240;
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
    rv = gfm_setBackground(pGame->pCtx, 0xff45283c);
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
    rv = gfm_addVirtualKey(&(pGame->handle_##handle), pGame->pCtx); \
    ASSERT(rv == GFMRV_OK, rv); \
    rv = gfm_bindInput(pGame->pCtx, pGame->handle_##handle, key); \
    ASSERT(rv == GFMRV_OK, rv);
#define BIND_KEY(handle, key) \
    rv = gfm_bindInput(pGame->pCtx, pGame->handle_##handle, key); \
    ASSERT(rv == GFMRV_OK, rv);
    
    // TODO Bind default gamepad buttons
    BIND_NEW_KEY(down, gfmKey_down);
    BIND_KEY(down, gfmKey_s);
    BIND_NEW_KEY(left, gfmKey_left);
    BIND_KEY(left, gfmKey_a);
    BIND_NEW_KEY(right, gfmKey_right);
    BIND_KEY(right, gfmKey_d);
    BIND_NEW_KEY(up, gfmKey_up);
    BIND_KEY(up, gfmKey_w);
    BIND_NEW_KEY(atk, gfmKey_x);
    BIND_KEY(atk, gfmKey_space);
    BIND_NEW_KEY(quit, gfmKey_esc);
    
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
#if !defined(EMSCRIPT)
    rv = gfm_playAudio(0, pGame->pCtx, pGame->song, 0.8);
    ASSERT(rv == GFMRV_OK, rv);
#endif
    
    // Set the initial state
    pGame->state = state_menustate;
    if (doSkip) {
        pGame->state = state_playstate;
    }
    // Set the quit state flag, so the first state is actually initialized
    pGame->quitState = 1;
    
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
        gfmGroup_free(&(pGame->pRender));
        gfmGenArr_clean(pGame->pObjs, gfmObject_free);
        gfmQuadtree_free(&(pGame->pQt));
        gfm_free(&(pGame->pCtx));
        // Clear the actual game context
        free(pGame);
    }
#endif
    return rv;
}

#if 0

#ifdef EMSCRIPT
#define DESPAIR_LOG(msg, ...) printf(msg, ##__VA_ARGS__)
#else
#define DESPAIR_LOG(msg, ...) do {} while(0)
#endif

#ifdef EMSCRIPT
#include <emscripten.h>

static gameCtx game;

void main_loop(void *pArg) {
    gameCtx *pGame;
    gfmRV rv;
    
    pGame = (gameCtx*)pArg;
    
    rv = gfm_issueFrame(pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Run the current state
    switch (pGame->state) {
        case state_introstate: introstate_loop(pGame); break;
        case state_blastate:   blastate_loop(pGame);   break;
        case state_playstate:  playstate_loop(pGame);  break;
        default: {}
    }
    
    rv = gfm_waitFrame(pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
__ret:
    if (rv != GFMRV_OK) {
        printf("Got error %i\n", rv);
    }
}
#endif

gfmRV main_cleanRenderGroup(gameCtx *pGame) {
    gfmRV rv;
    
    gfmGroup_free(&(pGame->pRender));
    
    rv = gfmGroup_getNew(&(pGame->pRender));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefSpriteset(pGame->pRender, pGame->pSset32x32);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefDimensions(pGame->pRender, 32/*width*/, 32/*height*/,
        0/*offX*/, 0/*offY*/);
    rv = gfmGroup_setDeathOnLeave(pGame->pRender, 0/*dontDie*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDeathOnTime(pGame->pRender, 0/*ttl*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDrawOrder(pGame->pRender, gfmDrawOrder_topFirst);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_preCache(pGame->pRender, 12, 0);
    ASSERT(rv == GFMRV_OK, rv);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

int main_getPRNG(gameCtx *pGame) {
    long int tmp = pGame->seed;
    
    tmp *= 0x19660d;
    tmp += 0x3c6ef35f;
    pGame->seed = tmp;
    
    return pGame->seed;
}

/**
 * Update all key's states (and the quit flag)
 */
gfmRV main_getKeyStates(gameCtx *pGame) {
    gfmRV rv;
    
#define GET_KEY_STATE(key) \
    rv = gfm_getKeyState(&(pGame->state_##key), &(pGame->num_##key), \
        pGame->pCtx, pGame->handle_##key); \
    ASSERT(rv == GFMRV_OK, rv)
    
    GET_KEY_STATE(down);
    GET_KEY_STATE(left);
    GET_KEY_STATE(right);
    GET_KEY_STATE(up);
    GET_KEY_STATE(atk);
    GET_KEY_STATE(quit);
    
#undef GET_KEY_STATE
    
    if ((pGame->state_quit & gfmInput_justPressed) == gfmInput_justPressed) {
        rv = gfm_setQuitFlag(pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
    }
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Load all assets; It should run on another thread while the main plays an
 * animations, but I didn't get to implement that... yet... XD
 * 
 * @param  pGame  The game
 * @return        ...
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
    rv = gfm_createSpritesetCached(&(pGame->pSset4x4), pGame->pCtx, texIndex,
        4/*tileWidth*/, 4/*tileHeight*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_createSpritesetCached(&(pGame->pSset8x8), pGame->pCtx, texIndex,
        8/*tileWidth*/, 8/*tileHeight*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_createSpritesetCached(&(pGame->pSset32x32), pGame->pCtx, texIndex,
        32/*tileWidth*/, 32/*tileHeight*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_createSpritesetCached(&(pGame->pSset256x128), pGame->pCtx,
        texIndex, 256/*tileWidth*/, 128/*tileHeight*/);
    ASSERT(rv == GFMRV_OK, rv);
    
#ifdef EMSCRIPT
#else
    rv = gfm_loadAudio(&(pGame->song), pGame->pCtx, "mysong.wav", 10);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_setRepeat(pGame->pCtx, pGame->song, 6 * pGame->audioFreq );
    ASSERT(rv == GFMRV_OK, rv);
#endif
    
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
    
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

int main(int argc, char *argv[]) {
#ifdef EMSCRIPT
#else
    gameCtx game;
#endif
    gfmAudioQuality audSettings;
    gfmInput *pInput;
    gfmRV rv;
    int bbufWidth, bbufHeight, doSkip, dps, fps, height, isFullscreen, ups,
            width;
    
    DESPAIR_LOG("Hero's Quest - by GFM\n");
    
    // Clean everything before hand
    memset(&game, 0x0, sizeof(gameCtx));
    
    // Start the library
    DESPAIR_LOG("Getting game's context...");
    rv = gfm_getNew(&(game.pCtx));
    ASSERT(rv == GFMRV_OK, rv);
    DESPAIR_LOG(" OK\n");
    DESPAIR_LOG("Initializing framework...");
    rv = gfm_initStatic(game.pCtx, "com.gfmgamecorner", "HerosQuest");
    ASSERT(rv == GFMRV_OK, rv);
    DESPAIR_LOG(" OK\n");
    
    // Intialize the seed
    game.seed = (unsigned int)time(0);
    DESPAIR_LOG("Set game seed!\n");
    
    // 'Parse' options
    isFullscreen = 0;
    width = 640;
    height = 480;
    game.maxParts = 2048;
    game.audioFreq = 44100;
    audSettings = gfmAudio_defQuality;
    doSkip = 0;
    
#ifndef EMSCRIPT
    while (argc > 1) {
        #define GETARG(opt) strcmp(argv[argc - 1], opt) == 0
        
        if (GETARG("-full") || GETARG("-f")) {
            isFullscreen = 1;
        }
        else if (GETARG("-skip") || GETARG("-s")) {
            doSkip = 1;
        }
        else if (GETARG("-noaudio") || GETARG("-m")) {
            rv =  gfm_disableAudio(game.pCtx);
            ASSERT(rv == GFMRV_OK, rv);
        }
        else if (GETARG("-badaudio") || GETARG("-l")) {
            // TODO Test with lowQuality
            audSettings = gfmAudio_medQuality;
            game.audioFreq = 22050;
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
    
    // TODO Remove this
#ifdef EMSCRIPT
    //DESPAIR_LOG("Disabling audio...");
    //rv =  gfm_disableAudio(game.pCtx);
    //ASSERT(rv == GFMRV_OK, rv);
    //DESPAIR_LOG(" OK!\n");
#endif
    
    // Create the window
    bbufWidth = 160;
    bbufHeight = 120;
    DESPAIR_LOG("Initializing game window...");
    if (isFullscreen) {
        rv = gfm_initGameFullScreen(game.pCtx, bbufWidth, bbufHeight,
                0/*defRes*/, 0/*dontResize*/);
    }
    else {
        rv = gfm_initGameWindow(game.pCtx, bbufWidth, bbufHeight, width, height,
                0/*dontResize*/);
    }
    ASSERT(rv == GFMRV_OK, rv);
    DESPAIR_LOG(" OK!\n");
    
    // Set the BG color
    DESPAIR_LOG("Setting background color...");
    rv = gfm_setBackground(game.pCtx, 0xff45283c);
    ASSERT(rv == GFMRV_OK, rv);
    DESPAIR_LOG(" OK\n");
    
    // Initialize the audio system
    DESPAIR_LOG("Initializing audio...");
    rv = gfm_initAudio(game.pCtx, audSettings);
    ASSERT(rv == GFMRV_OK, rv);
    DESPAIR_LOG(" OK\n");
    
    // Shorten the double press window
    DESPAIR_LOG("Setting double input delay...");
    rv = gfm_getInput(&pInput, game.pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmInput_setMultiDelay(pInput, 150/*ms*/);
    ASSERT(rv == GFMRV_OK, rv);
    DESPAIR_LOG(" OK\n");
    
    // Bind keys
#define BIND_NEW_KEY(handle, key) \
    rv = gfm_addVirtualKey(&(game.handle_##handle), game.pCtx); \
    ASSERT(rv == GFMRV_OK, rv); \
    DESPAIR_LOG("  Added virtual key "#handle"\n"); \
    rv = gfm_bindInput(game.pCtx, game.handle_##handle, key); \
    ASSERT(rv == GFMRV_OK, rv); \
    DESPAIR_LOG("  Bound vkey "#handle" to "#key"\n")
#define BIND_KEY(handle, key) \
    rv = gfm_bindInput(game.pCtx, game.handle_##handle, key); \
    ASSERT(rv == GFMRV_OK, rv); \
    DESPAIR_LOG("  Bound vkey "#handle" to "#key"\n")
    
    DESPAIR_LOG("Binding keys...\n");
    BIND_NEW_KEY(down, gfmKey_down);
    BIND_KEY(down, gfmKey_s);
    BIND_NEW_KEY(left, gfmKey_left);
    BIND_KEY(left, gfmKey_a);
    BIND_NEW_KEY(right, gfmKey_right);
    BIND_KEY(right, gfmKey_d);
    BIND_NEW_KEY(up, gfmKey_up);
    BIND_KEY(up, gfmKey_w);
    BIND_NEW_KEY(atk, gfmKey_x);
    BIND_KEY(atk, gfmKey_space);
    BIND_NEW_KEY(quit, gfmKey_esc);
    DESPAIR_LOG("OK\n");
    
#undef BIND_NEW_KEY
#undef BIND_KEY
    
    // Load assets
    DESPAIR_LOG("Loading assets...");
    rv = loadAssets(&game);
    ASSERT(rv == GFMRV_OK, rv);
    DESPAIR_LOG(" OK\n");
    
    // Set FPS
    ups = 60;
    dps = 60;
    DESPAIR_LOG("Setting update and draw rate...");
    rv = gfm_setStateFrameRate(game.pCtx, ups, dps);
    ASSERT(rv == GFMRV_OK, rv);
    DESPAIR_LOG(" OK\n");
    
    // Set the timer resolution, in frames per seconds
    fps = 60;
    DESPAIR_LOG("Setting timer's callback");
    rv = gfm_setFPS(game.pCtx, fps);
    ASSERT(rv == GFMRV_OK, rv);
    DESPAIR_LOG(" OK\n");
    
    // Initialize the FPS counter (only visible in debug mode, though)
    DESPAIR_LOG("Initializing FPS counter...");
    rv = gfm_initFPSCounter(game.pCtx, game.pSset8x8, 0/*firstTile*/);
    ASSERT(rv == GFMRV_OK, rv);
    DESPAIR_LOG(" OK\n");
    
    // Initialize the quadtree
    DESPAIR_LOG("Initializing quadtree...");
    rv = gfmQuadtree_getNew(&(game.pQt));
    ASSERT(rv == GFMRV_OK, rv);
    DESPAIR_LOG(" OK\n");
    
    // Play the song
#ifdef EMSCRIPT
#else
    DESPAIR_LOG("Playing song...");
    rv = gfm_playAudio(0, game.pCtx, game.song, 0.8);
    ASSERT(rv == GFMRV_OK, rv);
    DESPAIR_LOG(" OK\n");
#endif
    
    // Loop...
    game.state = state_introstate;
    if (doSkip) {
        game.state = state_playstate;
    }
#ifdef EMSCRIPT
    DESPAIR_LOG("Setting main loop...");
    emscripten_set_main_loop_arg((em_arg_callback_func)main_loop, &game, 0, 0);
    DESPAIR_LOG(" OK\n");
#else
    while (gfm_didGetQuitFlag(game.pCtx) == GFMRV_FALSE) {
        // Run the current state
        switch (game.state) {
            case state_introstate: rv = introstate_loop(&game); break;
            case state_blastate: rv = blastate_loop(&game); break;
            case state_playstate: rv = playstate_loop(&game); break;
            default: rv = GFMRV_INTERNAL_ERROR;
        }
        ASSERT(rv == GFMRV_OK, rv);
        
        game.quitState = 0;
    }
#endif
    
    rv = GFMRV_OK;
__ret:
#ifdef EMSCRIPT
    // This avoids clearing resources before the game starts running
    DESPAIR_LOG("Done with initialization!\n");
    return rv;
#else
    // Clean all resources
    gfmGroup_free(&(game.pRender));
    gfmGenArr_clean(game.pObjs, gfmObject_free);
    gfmQuadtree_free(&(game.pQt));
    gfm_free(&(game.pCtx));
    
    return rv;
#endif
}

#endif /* 0 */

