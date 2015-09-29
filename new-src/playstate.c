/**
 * @file new-src/playstate.c
 * 
 * The game's playstate
 */
#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmCamera.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmGroup.h>
#include <GFraMe/gfmObject.h>
#include <GFraMe/gfmParser.h>
#include <GFraMe/gfmTilemap.h>
#include <GFraMe/gfmText.h>

#include <ld32_pc/game.h>
#include <ld32_pc/player.h>
#include <ld32_pc/stone.h>
#include <ld32_pc/ui.h>

#include <string.h>

static int pParticleAnim[] = {
/*                */ /*len|fps|loop|frames...*/
/*   RED_BULLET   */    20, 12,  0 , 256,257,258,259, 258,259,259,259, 259,259,259,259, 258,259,260,261, 262,263,264,265,
/* ORANGE_BULLET  */    20, 12,  0 , 266,267,268,269, 268,269,269,269, 269,269,269,269, 268,269,270,271, 272,273,274,275,
/* YELLOW_BULLET  */    20, 12,  0 , 276,277,278,279, 278,279,279,279, 279,279,279,279, 278,279,280,281, 282,283,284,285,
/*  GREEN_BULLET  */    20, 12,  0 , 288,289,290,291, 290,291,291,291, 291,291,291,291, 290,291,292,293, 294,295,296,297,
/*  CYAN_BULLET   */    20, 12,  0 , 298,299,300,301, 300,301,301,301, 301,301,301,301, 300,301,302,303, 304,305,306,307,
/*  BLUE_BULLET   */    20, 12,  0 , 308,309,310,311, 310,311,311,311, 311,311,311,311, 310,311,312,313, 314,315,316,317,
/* PURPLE_BULLET  */    20, 12,  0 , 320,321,322,323, 322,323,323,323, 323,323,323,323, 322,323,324,325, 326,327,328,329,
/* MAX_PART_ANIM  */     1, 0 ,  0 , -1
};
static int particleAnimlen = sizeof(pParticleAnim) / sizeof(int);

// Dictionary relates tiles and types
static char *typesNameDict[] = {
    "collideable",
    "spike",
    "checkpoint",
    "player",
    "stone"
};
static int typesValueDict[] = {
    tCollideable,
    tSpike,
    tCheckpoint,
    tPlayer,
    tPowerstone
};

// List of maps and object's file names
static char *mapsDict[] = {
    "map_0.gfm",
    "map_max"
};
static char *objsDict[] = {
    "objs_0.gfm",
    "map_max"
};

struct stPlaystateCtx {
    gfmGroup *pParticles;
    gfmObject *pLastCheckpoint;
    gfmTilemap *pTMap;
    gfmText *pText;
    int curMap;
    int curCheckpointX;
    int curCheckpointY;
    int mapWidth;
    int mapHeight;
    int stoneCount;
    player *pPlayer;
    stone *pStone[7];
};
typedef struct stPlaystateCtx psCtx;

/**
 * Clean up the playstate
 * 
 * @param  [in]pCtx The playstate
 */
static void stPs_clean(psCtx *pCtx) {
    // Avoid errors!
    if (!pCtx)
        return;
    
    // Clean everything!
    gfmGroup_free(&(pCtx->pParticles));
    gfmTilemap_free(&(pCtx->pTMap));
    gfmText_free(&(pCtx->pText));
    pl_clean(&(pCtx->pPlayer));
    
    while (pCtx->stoneCount > 0) {
        pCtx->stoneCount--;
        st_clean(&(pCtx->pStone[pCtx->stoneCount]));
    }
}

/**
 * Loads the current map from a file
 * 
 * @param  [in]pCtx The playstate
 */
static gfmRV stPs_loadMap(psCtx *pPsCtx, gameCtx *pGame) {
    char *pMapname, *pObjsname;
    gfmCamera *pCamera;
    gfmCtx *pCtx;
    gfmParser *pParser;
    gfmRV rv;
    int mapnameLen, objsnameLen;
    
    // Set these to NULL to avoid errors
    pMapname = 0;
    pObjsname = 0;
    pParser = 0;
    
    pCtx = pGame->pCtx;
    
    // Retrieve tha map's name (stored on a static buffer because C)
#define GETMAP(i) \
    do { \
        pMapname = mapsDict[i]; \
        pObjsname = objsDict[i]; \
        mapnameLen = strlen(mapsDict[i]); \
        objsnameLen = strlen(objsDict[i]); \
    } while (0)
    switch (pPsCtx->curMap) {
        case 0: GETMAP(0); break;
        default: {
            // TODO Load a "default" level
        }
    }
#undef GETMAP
    
    // Load the map
    rv = gfmTilemap_loadf(pPsCtx->pTMap, pCtx, pMapname, mapnameLen,
            typesNameDict, typesValueDict,
            sizeof(typesValueDict) / sizeof(int));
    ASSERT(rv == GFMRV_OK, rv);
    // TODO Load animations?
    
    // Update the camera's world space
    rv = gfm_getCamera(&pCamera, pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmTilemap_getDimension(&(pPsCtx->mapWidth), &(pPsCtx->mapHeight), pPsCtx->pTMap);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmCamera_setWorldDimensions(pCamera, pPsCtx->mapWidth, pPsCtx->mapHeight);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Parse its objects
    rv = gfmParser_getNew(&pParser);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmParser_init(pParser, pCtx, pObjsname, objsnameLen);
    ASSERT(rv == GFMRV_OK, rv);
    
    while (1) {
        gfmParserType type;
        
        // Retrieve the next object and check if parsing ended
        rv = gfmParser_parseNext(pParser);
        ASSERT(rv == GFMRV_OK || rv == GFMRV_PARSER_FINISHED, rv);
        if (rv == GFMRV_PARSER_FINISHED) {
            break;
        }
        
        // Get all of the object's type
        rv = gfmParser_getType(&type, pParser);
        ASSERT(rv == GFMRV_OK, rv);
        
        // Spawn the object
        if (type == gfmParserType_area) {
            int h, w, x, y;
            
            rv = gfmParser_getPos(&x, &y, pParser);
            ASSERT(rv == GFMRV_OK, rv);
            rv = gfmParser_getDimensions(&w, &h, pParser);
            ASSERT(rv == GFMRV_OK, rv);
            
            // TODO Alloc the object!
        }
        else if (type == gfmParserType_object) {
            char *pType;
            
            rv = gfmParser_getIngameType(&pType, pParser);
            ASSERT(rv == GFMRV_OK, rv);
            
            if (strcmp(pType, "player") == 0) {
                int x, y;
                
                rv = pl_init(&(pPsCtx->pPlayer), pGame, pParser);
                ASSERT(rv == GFMRV_OK, rv);
                
                // Store the player's initial position
                rv = gfmParser_getPos(&x, &y, pParser);
                ASSERT(rv == GFMRV_OK, rv);
                
                pPsCtx->curCheckpointX = x;
                pPsCtx->curCheckpointY = y;
            }
            else if (strcmp(pType, "stone") == 0) {
                stone *pStone;
                
                ASSERT(pPsCtx->stoneCount < 7, GFMRV_FUNCTION_FAILED);
                
                pStone = 0;
                rv = st_init(&pStone, pGame, pParser);
                ASSERT(rv == GFMRV_OK, rv);
                
                pPsCtx->pStone[pPsCtx->stoneCount] = pStone;
                pPsCtx->stoneCount++;
            }
            else {
                // Force an error
                ASSERT(0, GFMRV_PARSER_BAD_TOKEN);
            }
        }
    }
    
    rv = GFMRV_OK;
__ret:
    gfmParser_free(&pParser);
    
    return rv;
}

/**
 * Initialize the playstate: stores the playstate context, loads the map, etc
 * 
 * @param  [in]pGame The game context
 * @return           GFMRV_OK, GFMRV_ALLOC_FAILED, ...
 */
gfmRV ps_init(gameCtx *pGame) {
    gfmCamera *pCamera;
    gfmRV rv;
    psCtx *pCtx;
    
    // Alloc the state
    pCtx = (psCtx*)malloc(sizeof(psCtx));
    ASSERT(pCtx, GFMRV_ALLOC_FAILED);
    memset(pCtx, 0x0, sizeof(psCtx));
    
    // Make the camera smaller than the screen, to leave UI space at the bottom
    rv = gfm_getCamera(&pCamera, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    // TODO Fix the UI height
    rv = gfmCamera_init(pCamera, pGame->pCtx, GAME_BBUF_WIDTH,
            GAME_BBUF_HEIGHT - GAME_UI_HEIGHT);
    ASSERT(rv == GFMRV_OK, rv);
    
    if (pGame->maxParticles > 0) {
        // Alloc the particles and set its default attributes
        rv = gfmGroup_getNew(&(pCtx->pParticles));
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmGroup_setDefSpriteset(pCtx->pParticles, pGame->pSset8x8);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmGroup_setDefDimensions(pCtx->pParticles, 8, 8, -3, -3);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmGroup_setDefAnimData(pCtx->pParticles, pParticleAnim,
                particleAnimlen);
        ASSERT(rv == GFMRV_OK, rv);
        rv =  gfmGroup_setDeathOnLeave(pCtx->pParticles, 0);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmGroup_setDeathOnTime(pCtx->pParticles, 4000);
        ASSERT(rv == GFMRV_OK, rv);
        // Pre-cache some sprites
        rv = gfmGroup_preCache(pCtx->pParticles, pGame->maxParticles,
                pGame->maxParticles);
        ASSERT(rv == GFMRV_OK, rv);
    }
    
    // Alloc and initialize the tilemap
    rv = gfmTilemap_getNew(&(pCtx->pTMap));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmTilemap_init(pCtx->pTMap, pGame->pSset8x8, 8, 8, -1);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Alloc and initialize the text
    rv = gfmText_getNew(&(pCtx->pText));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmText_init(pCtx->pText, 8, 8, 304, 3, 166, 0, pGame->pSset8x8, 0);
    ASSERT(rv == GFMRV_OK, rv);
    
    if (0) {
        // TODO Load the current level from a file(?)
    }
    else {
        // Set the first map
        pCtx->curMap = 0;
    }
    
    // Load the first map
    rv = stPs_loadMap(pCtx, pGame);
    ASSERT(rv == GFMRV_OK, rv);
    
    pGame->pState = pCtx;
    pCtx = 0;
    rv = GFMRV_OK;
__ret:
    if (pCtx) {
        // Clean the context
        stPs_clean(pCtx);
        free(pCtx);
        
        pCtx = 0;
    }
    
    return rv;
}

/**
 * Updates the playstate a single frame
 * 
 * @param  [in]pGame The game context
 * @return           GFMRV_OK, ...
 */
gfmRV ps_update(gameCtx *pGame) {
    psCtx *pPsCtx;
    gfmCtx *pCtx;
    gfmRV rv;
    int i;
    
    // Retrieve the context
    pPsCtx = pGame->pState;
    pCtx = pGame->pCtx;
    
    // Ready the quadtree to collide with everything
    rv = gfmQuadtree_initRoot(pGame->pQt, -8/*x*/, -8/*y*/, pPsCtx->mapWidth + 16,
        pPsCtx->mapHeight + 16, 6/*maxDepth*/, 10/*maxNodes*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmQuadtree_populateTilemap(pGame->pQt, pPsCtx->pTMap);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Update everything
    rv = gfmTilemap_update(pPsCtx->pTMap, pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = pl_update(pPsCtx->pPlayer, pGame);
    ASSERT(rv == GFMRV_OK, rv);
    i = 0;
    while (i < pPsCtx->stoneCount) {
        st_update(pPsCtx->pStone[i], pGame);
        i++;
    }
    
    // Update particles; can be done in another step since it's simply aesthetic
    if (pPsCtx->pParticles) {
        rv = gfmGroup_update(pPsCtx->pParticles, pCtx);
        ASSERT(rv == GFMRV_OK, rv);
    }
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Draw a single frame from the playstate
 * 
 * @param  [in]pGame The game context
 * @return           GFMRV_OK, ...
 */
gfmRV ps_draw(gameCtx *pGame) {
    psCtx *pPsCtx;
    gfmCtx *pCtx;
    gfmRV rv;
    int i;
    
    // Retrieve the context
    pPsCtx = pGame->pState;
    pCtx = pGame->pCtx;
    
    // Draw everything
    rv = gfmTilemap_draw(pPsCtx->pTMap, pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = pl_draw(pPsCtx->pPlayer, pGame);
    ASSERT(rv == GFMRV_OK, rv);
    i = 0;
    while (i < pPsCtx->stoneCount) {
        st_draw(pPsCtx->pStone[i], pGame);
        i++;
    }
    
    // TODO Draw everything
    
    // Finally, draw the particles
    if (pPsCtx->pParticles) {
        rv = gfmGroup_draw(pPsCtx->pParticles, pCtx);
        ASSERT(rv == GFMRV_OK, rv);
    }
    
    // Draw the quadtree's rectangles (for debugging only)
    //rv = gfmQuadtree_drawBounds(pGame->pQt, pCtx, 0);
    //ASSERT(rv == GFMRV_OK, rv);
    
    rv = ui_draw(pGame);
    ASSERT(rv == GFMRV_OK, rv);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Clean up all alloc'ed memory
 * 
 * @param  [in]pGame The game context
 * @return           GFMRV_OK, ...
 */
gfmRV ps_clean(gameCtx *pGame) {
    psCtx *pPsCtx;
    
    pPsCtx = pGame->pState;
    stPs_clean(pPsCtx);
    
    return GFMRV_OK;
}

/**
 * Retrieve the player's starting position
 * 
 * @param  [in]pX    The horizontal position
 * @param  [in]pY    The vertical position
 * @param  [in]pGame The game context
 * @return           GFMRV_OK, ...
 */
gfmRV ps_getPlayerInitPos(int *pX, int *pY, gameCtx *pGame) {
    psCtx *pPsCtx;
    
    pPsCtx = pGame->pState;
    
    *pX = pPsCtx->curCheckpointX;
    *pY = pPsCtx->curCheckpointY;
    
    return GFMRV_OK;
}

/**
 * Store the current checkpoint, if the player overlaps against it
 * 
 * @param  [in]pPl   The player
 * @param  [in]pGame The game context
 * @param  [in]pCp   The checkpoint
 * @return           GFMRV_OK, ...
 */
gfmRV ps_plCollideCheckpoint(void *pPlayer, gameCtx *pGame, void *pCp) {
    gfmObject *pObj;
    gfmRV rv;
    int cx, cy;
    psCtx *pPsCtx;
    
    pPsCtx = pGame->pState;
    pObj = (gfmObject*)pCp;
    
    rv = gfmObject_getCenter(&cx, &cy, pObj);
    ASSERT(rv == GFMRV_OK, rv);
    
    if (pPsCtx->curCheckpointX != cx && pPsCtx->curCheckpointY != cy) {
        pPsCtx->curCheckpointX = cx;
        pPsCtx->curCheckpointY = cy;
        
        // TODO Spawn particles
        // TODO Play sound
        // TODO Display message "Checkpoint!"
    }
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Retrive the state's particle group
 * 
 * @param [out]ppGrp The retrieved group
 * @param [in] pGame The game context
 * @return           GFMRV_OK, ...
 */
gfmRV ps_getParticles(gfmGroup **ppGrp, gameCtx *pGame) {
    gfmRV rv;
    psCtx *pPsCtx;
    
    // TODO Check state?
    pPsCtx = pGame->pState;
    
    *ppGrp = pPsCtx->pParticles;
    
    rv = GFMRV_OK;
    return rv;
}

/**
 * Retrieve the player's reference
 * 
 * @param  [out]ppPlayer The retrieved player
 * @param  [in] pGame    The game context
 * @return               GFMRV_OK, ...
 */
gfmRV ps_getPlayer(player **ppPlayer, gameCtx *pGame) {
    gfmRV rv;
    psCtx *pPsCtx;
    
    // TODO Check state?
    pPsCtx = pGame->pState;
    
    *ppPlayer = pPsCtx->pPlayer;
    
    rv = GFMRV_OK;
    return rv;
}

#if 0
#include <GFraMe/GFraMe_controller.h>
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_event.h>
#include <GFraMe/GFraMe_keys.h>
#include <GFraMe/GFraMe_object.h>
#include <GFraMe/GFraMe_spriteset.h>

GFraMe_event_setup();

#include "audio.h"
#include "camera.h"
#include "global.h"
#include "map001.h"
#include "player.h"
#include "playstate.h"
#include "sprite.h"
#include "text.h"
#include "ui.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

/** Variable to enable drawing of hitbox */
#ifdef DEBUG
extern int GFraMe_draw_debug;
#endif

struct stPlaystate {
    /** Camera object */
    camera *pCam;
    /** Check for how long the player has been in the dead zone */
    int timeInDeadZone;
    /** The player */
    player *pPl;
    /** The stones of powah */
    sprite **pStones;
    /** The text */
    text *pText;
    /**  How many stones there are in use */
    int stonesUsed;
    /** How many stones there are allocated */
    int stonesLen;
    /** The spikes of powah */
    sprite **pSpikes;
    /**  How many spikes there are in use */
    int spikesUsed;
    /** How many spikes there are allocated */
    int spikesLen;
    /** The player's bullets */
    sprite **pPlBullets;
    /** How many plBullets there are allocated */
    int plBulletsLen;
    /** The bounds of the stage */
    GFraMe_object *pWalls;
    /**  How many walls there are in use */
    int wallsUsed;
    /** How many walls there are allocated */
    int wallsLen;
    /** Index of the current map */
    int curMap;
    /** Map width, in tiles */
    int mapWidth;
    /** Map height, in tiles */
    int mapHeight;
    /** How long the player has been dead */
    int plDeadTimer;
    /** Map's tilemap */
    unsigned char *mapBuf;
    /** Current state */
    int state;
#ifdef DEBUG
    int skippedFrames;
#endif /* DEBUG */
};

void ps_event(struct stPlaystate *pPs);
int ps_setMap(struct stPlaystate *pPs, int map);
void ps_drawMap(struct stPlaystate *pPs);

int ps_init(struct stPlaystate *pPs) {
    int rv;
    
    // Initialize the player
    rv = pl_getNew(&pPs->pPl);
    ASSERT_NR(rv == 0);
    rv = pl_init(pPs->pPl, PL_X, PL_Y);
    ASSERT_NR(rv == 0);
    
    // Initialize the text
    rv = txt_getNew(&pPs->pText);
    ASSERT_NR(rv == 0);
    
    // Initialize the camera
    rv = cam_getNew(&pPs->pCam);
    ASSERT_NR(rv == 0);
    
    // Get the current map
    rv = ps_setMap(pPs, 0);
    ASSERT_NR(rv == 0);
    
    // Initialize the timer
    GFraMe_event_init(UPS, DPS);
    
    txt_setText(pPs->pText, 0);
    
    aud_playSong();
    
    rv = 0;
__ret:
    return rv;
}

void ps_update(struct stPlaystate *pPs) {
#ifdef DEBUG
  pPs->skippedFrames = 1;
  if (GFraMe_controller_max > 0 && GFraMe_controllers[0].l3)
      pPs->skippedFrames = 4;
#endif
  GFraMe_event_update_begin();
#ifdef DEBUG
while (pPs->skippedFrames > 0) {
#endif
    int i;
    
#ifdef DEBUG
    if (GFraMe_keys.r || (GFraMe_controller_max && GFraMe_controllers[0].a)) {
        pl_revive(pPs->pPl);
    }
    if (GFraMe_keys.one)
        pl_addStone(pPs->pPl, SPR_RED_STONE);
    if (GFraMe_keys.two)
        pl_addStone(pPs->pPl, SPR_ORANGE_STONE);
    if (GFraMe_keys.three)
        pl_addStone(pPs->pPl, SPR_YELLOW_STONE);
    if (GFraMe_keys.four)
        pl_addStone(pPs->pPl, SPR_GREEN_STONE);
    if (GFraMe_keys.five)
        pl_addStone(pPs->pPl, SPR_CYAN_STONE);
    if (GFraMe_keys.six)
        pl_addStone(pPs->pPl, SPR_BLUE_STONE);
    if (GFraMe_keys.seven)
        pl_addStone(pPs->pPl, SPR_PURPLE_STONE);
#endif
    if (!pl_isAlive(pPs->pPl)) {
        pPs->plDeadTimer += GFraMe_event_elapsed;
        if (pPs->plDeadTimer > RESPAWN_TIME) {
            pl_revive(pPs->pPl);
            pPs->plDeadTimer = 0;
        }
    }
    // Update everything
    pl_update(pPs->pPl, pPs->pCam, GFraMe_event_elapsed);
    if (pl_isShooting(pPs->pPl) || pPs->state == 7) {
        double ang, dang;
        int n;
        int iniX, iniY, sX, sY;
        sprType stones, curStone;
        
        if (pPs->state < 7) {
            pl_getShotParams(&iniX, &iniY, &sX, &sY, &stones, pPs->pPl);
            
            ang = atan2((double)sX/PL_BUL_SPEED, (double)-sY/PL_BUL_SPEED);
            ang -= PI / 2.0;
            
            n = 0;
            curStone = 1;
            while (curStone < 0x0100) {
                if (curStone & stones)
                    n++;
                curStone <<= 1;
            }
            dang = PL_BUL_DANG * PI / 180.0;
            if (n > 0)
                ang -= dang * n / 2.0;
            sX = PL_BUL_SPEED*cos(ang);
            sY = PL_BUL_SPEED*sin(ang);
        }
        else {
            pl_getShotParams(&iniX, &iniY, &sX, &sY, &stones, pPs->pPl);
            
            dang = PL_BUL_DANG * PI / 180.0;
            ang = -PI / 2.0 - dang * 7 / 2.0;
            sX = PL_BUL_SPEED*cos(ang);
            sY = PL_BUL_SPEED*sin(ang);
        }
        
        curStone = 1;
        while (curStone < 0x0100) {
            GFraMe_sprite *pGfmSpr;
            int *animData, animLen, rv;
            sprite *pSpr;
            
            if (!(curStone & stones)) goto __next_stone;
            
            rv = spr_recycle(&pSpr, &pPs->pPlBullets, &pPs->plBulletsLen);
            if (rv != 0) goto __next_stone;
            
            switch (curStone) {
                case SPR_RED_STONE:
                    animData = _sprRedBulAnimData;
                    animLen = _sprRedBulAnimLen;
                break;
                case SPR_ORANGE_STONE:
                    animData = _sprOrangeBulAnimData;
                    animLen = _sprOrangeBulAnimLen;
                break;
                case SPR_YELLOW_STONE:
                    animData = _sprYellowBulAnimData;
                    animLen = _sprYellowBulAnimLen;
                break;
                case SPR_GREEN_STONE:
                    animData = _sprGreenBulAnimData;
                    animLen = _sprGreenBulAnimLen;
                break;
                case SPR_CYAN_STONE:
                    animData = _sprCyanBulAnimData;
                    animLen = _sprCyanBulAnimLen;
                break;
                case SPR_BLUE_STONE:
                    animData = _sprBlueBulAnimData;
                    animLen = _sprBlueBulAnimLen;
                break;
                case SPR_PURPLE_STONE:
                    animData = _sprPurpleBulAnimData;
                    animLen = _sprPurpleBulAnimLen;
                break;
                default: {
                    animData = 0;
                    animLen = 0;
                }
            }
            
            rv = spr_init(pSpr, iniX, iniY, 0/*offX*/, 0/*offY*/, 4/*width*/,
                    4/*height*/, 4/*hitboxWidth*/, 4/*hitboxHeight*/, animData,
                    animLen, curStone);
            if (rv != 0) goto __next_stone;
            
            spr_getSprite(&pGfmSpr, pSpr);
            pGfmSpr->obj.vx = sX;
            pGfmSpr->obj.vy = sY;
            if (pPs->state == 7) {
                pGfmSpr->obj.ay = -sY;
            }
            
__next_stone:
            ang += dang;
            sX = PL_BUL_SPEED*cos(ang);
            sY = PL_BUL_SPEED*sin(ang);
            curStone <<= 1;
        }
    }
    i = 0;
    while (i < pPs->stonesUsed) {
        spr_update(pPs->pStones[i], GFraMe_event_elapsed);
        i++;
    }
    i = 0;
    while (i < pPs->plBulletsLen) {
        spr_update(pPs->pPlBullets[i], GFraMe_event_elapsed);
        if (!spr_isInsideCamera(pPs->pPlBullets[i], pPs->pCam))
            spr_kill(pPs->pPlBullets[i]);
        i++;
    }
    
    // Collide everything
    pl_collideAgainstGroup(pPs->pPl, pPs->pWalls, pPs->wallsLen,
        0 /*isPlFixed*/, 1/*isObjsFixed*/);
    pl_collideAgainstSprGroup(pPs->pPl, pPs->pStones, pPs->stonesUsed,
        0 /*isPlFixed*/, 0/*isObjsFixed*/);
    pl_collideAgainstSprGroup(pPs->pPl, pPs->pSpikes, pPs->spikesUsed,
        0 /*isPlFixed*/, 0/*isObjsFixed*/);
    
    {
        int num;
        pl_getStoneCount(&num, pPs->pPl);
        
        if (pPs->state < num) {
            pPs->state++;
            txt_setText(pPs->pText, pPs->state);
        }
    }
    txt_update(pPs->pText, GFraMe_event_elapsed);
    // Update the camera's position
    {
        int x, y, rv, w, h;
        
        pl_getCenter(&x, &y, pPs->pPl);
        
        rv = cam_centerAt(pPs->pCam, x, y);
        if (rv && pPs->timeInDeadZone < CAM_DEADZONE_TIME) {
            pPs->timeInDeadZone += GFraMe_event_elapsed;
        }
        else if (!rv && pPs->timeInDeadZone > 0) {
            pPs->timeInDeadZone -= GFraMe_event_elapsed;
        }
        w = (SCRW * CAM_MAX_RATIO) * (CAM_DEADZONE_TIME - pPs->timeInDeadZone) /
                CAM_DEADZONE_TIME + (SCRW * CAM_MIN_RATIO) * pPs->timeInDeadZone /
                    CAM_DEADZONE_TIME;
        if (w < SCRW * CAM_MIN_RATIO)
            w = SCRW * CAM_MIN_RATIO;
        h = (SCRH * CAM_MAX_RATIO) * (CAM_DEADZONE_TIME - pPs->timeInDeadZone) /
                CAM_DEADZONE_TIME + (SCRH * CAM_MIN_RATIO) * pPs->timeInDeadZone /
                    CAM_DEADZONE_TIME;
        if (h < SCRW * CAM_MIN_RATIO)
            h = SCRW * CAM_MIN_RATIO;
        
        cam_setDeadzone(pPs->pCam, w, h);
    }
#ifdef DEBUG
    pPs->skippedFrames--;
}
#endif
  GFraMe_event_update_end();
#ifdef DEBUG
    pPs->skippedFrames = 0;
#endif
}

void ps_draw(struct stPlaystate *pPs) {
  GFraMe_event_draw_begin();
    int i;
    
    ps_drawMap(pPs);
    
    i = 0;
    while (i < pPs->stonesUsed) {
        spr_draw(pPs->pStones[i], pPs->pCam);
        i++;
    }
    i = 0;
    while (i < pPs->plBulletsLen) {
        spr_draw(pPs->pPlBullets[i], pPs->pCam);
        i++;
    }
    pl_draw(pPs->pPl, pPs->pCam);
    ui_draw(pPs->pPl);
    txt_draw(pPs->pText);
  GFraMe_event_draw_end();
}

void ps_clean(struct stPlaystate *pPs) {
    if (pPs->pPl)
        pl_free(&pPs->pPl);
    if (pPs->pCam)
        cam_free(&pPs->pCam);
    if (pPs->pText)
        txt_free(&pPs->pText);
    if (pPs->pStones) {
        int i;
        
        i = 0;
        while (i < pPs->stonesLen) {
            spr_free(&(pPs->pStones[i]));
            i++;
        }
        free(pPs->pStones);
        pPs->pStones = 0;
    }
    if (pPs->pSpikes) {
        int i;
        
        i = 0;
        while (i < pPs->spikesLen) {
            spr_free(&(pPs->pSpikes[i]));
            i++;
        }
        free(pPs->pSpikes);
        pPs->pSpikes = 0;
    }
    if (pPs->pPlBullets) {
        int i;
        
        i = 0;
        while (i < pPs->plBulletsLen) {
            spr_free(&(pPs->pPlBullets[i]));
            i++;
        }
        free(pPs->pPlBullets);
        pPs->pPlBullets = 0;
    }
    if (pPs->pWalls) {
        free(pPs->pWalls);
        pPs->pWalls = 0;
    }
}

void playstate() {
    int rv;
    struct stPlaystate *pPs;
    
    // Alloc the playstate structure
    pPs = (struct stPlaystate*) malloc(sizeof(struct stPlaystate));
    ASSERT_NR(pPs);
    memset(pPs, 0, sizeof(struct stPlaystate));
    
    rv = ps_init(pPs);
    ASSERT_NR(rv == 0);

    while (gl_running) {
        ps_event(pPs);
        ps_update(pPs);
        ps_draw(pPs);
    }
__ret:
    // Clean everything
    if (pPs) {
        ps_clean(pPs);
        free(pPs);
        pPs = 0;
    }
    
    return;
}

void ps_event(struct stPlaystate *pPs) {
  GFraMe_event_begin();
    GFraMe_event_on_timer();
      GFraMe_event_on_mouse_up();
      GFraMe_event_on_mouse_down();
      GFraMe_event_on_mouse_moved();
    //  GFraMe_event_on_finger_down();
    //  GFraMe_event_on_finger_up();
    GFraMe_event_on_controller();
#ifdef DEBUG
      if (GFraMe_controller_max > 0 && GFraMe_controllers[0].home)
        gl_running = 0;
      if (GFraMe_controller_max > 0 && GFraMe_controllers[0].l2) {
        GFraMe_draw_debug = 1;
      }
      else {
        GFraMe_draw_debug = 0;
      }
#endif
    GFraMe_event_on_key_down();
#ifdef DEBUG
      if (GFraMe_keys.esc)
        gl_running = 0;
      if (GFraMe_keys.f1) {
        GFraMe_draw_debug = 1;
      }
      else {
        GFraMe_draw_debug = 0;
      }
#endif
    GFraMe_event_on_key_up();
    GFraMe_event_on_quit();
      gl_running = 0;
  GFraMe_event_end();
}

int ps_setMap(struct stPlaystate *pPs, int map) {
    int rv;
    
    pPs->wallsUsed = 0;
    pPs->stonesUsed = 0;
    pPs->spikesUsed = 0;
    switch (map) {
        default: {
            // TODO put this in a macro (only if there are more maps)
            // Assign the current map
            pPs->curMap = 1;
            // Get the map's collision bounds
            rv = map001_getWalls(&pPs->pWalls, &pPs->wallsLen, &pPs->wallsUsed);
            ASSERT_NR(rv == 0);
            // Get the map's data
            pPs->mapWidth = map001_width;
            pPs->mapHeight = map001_height;
            pPs->mapBuf = (unsigned char*)map001_tilemap;
            // Get the stones of power
            rv = map001_getStones(&pPs->pStones, &pPs->stonesLen, &pPs->stonesUsed);
            ASSERT_NR(rv == 0);
            // Get the spikes
            rv = map001_getSpikes(&pPs->pSpikes, &pPs->spikesLen, &pPs->spikesUsed);
            ASSERT_NR(rv == 0);
        }
    }
    
    // TODO do something if the map is smaller than the screen
    cam_init(pPs->pCam, SCRW, SCRH, pPs->mapWidth * 8, pPs->mapHeight * 8);
    
    rv = 0;
__ret:
    return rv;
}

void ps_drawMap(struct stPlaystate *pPs) {
    int camX, camY, camW, camH, firstTile, dX, i, iniX, offX, x, y;
    
    // TODO do something if the map is smaller than the screen
    
    cam_getParams(&camX, &camY, &camW, &camH, pPs->pCam);
    
    // Get the first tile's position on the screen
    iniX = -(camX % 8);
    y = -(camY % 8);
    // Get the first tile on the screen
    firstTile = camX / 8 + camY / 8 * pPs->mapWidth;
    // Get how many tiles are skipped each row
    dX = pPs->mapWidth - camW / 8;
    // if the camera's position doesn't match a tile, it will render 1 extra tile
    if (iniX != 0)
        dX--;
    
    // Loop through every tile
    i = 0;
    x = iniX;
    offX = 0;
    while (1) {
        // CHeck that the tile is still valid
        if (i >= pPs->mapWidth * pPs->mapHeight)
            break;
        // Render the tile to the screen
        GFraMe_spriteset_draw(gl_sset8x8, pPs->mapBuf[firstTile + offX + i], x,
                y, 0 /* flipped */);
        // Updates the tile positions
        x += 8;
        if (x >= camW) {
            x = iniX;
            y += 8;
            offX += dX;
        }
        if (y > camH)
            break;
        i++;
    }
}

#endif

