/**
 * @file src/playstate.c
 */
#include <GFraMe/GFraMe_controller.h>
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_event.h>
#include <GFraMe/GFraMe_keys.h>
#include <GFraMe/GFraMe_object.h>
#include <GFraMe/GFraMe_spriteset.h>

GFraMe_event_setup();

#include "camera.h"
#include "global.h"
#include "player.h"
#include "playstate.h"
#include "map001.h"

#include <stdlib.h>
#include <string.h>

#define PL_X 32
#define PL_Y 32

/** Variable to enable drawing of hitbox */
#ifdef DEBUG
extern int GFraMe_draw_debug;
#endif

struct stPlaystate {
    /** Camera object */
    camera *pCam;
    /** The player */
    player *pPl;
    /** The bounds of the stage */
    GFraMe_object *pWalls;
    /** Index of the current map */
    int curMap;
    /**  How many walls there are in use */
    int wallsUsed;
    /** How many walls there are allocated */
    int wallsLen;
    /** Map width, in tiles */
    int mapWidth;
    /** Map height, in tiles */
    int mapHeight;
    /** Map's tilemap */
    unsigned char *mapBuf;
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
    
    // Initialize the camera
    rv = cam_getNew(&pPs->pCam);
    ASSERT_NR(rv == 0);
    
    // Get the current map
    rv = ps_setMap(pPs, 0);
    ASSERT_NR(rv == 0);
    
    // Initialize the timer
    GFraMe_event_init(UPS, DPS);
    
    rv = 0;
__ret:
    return rv;
}

void ps_update(struct stPlaystate *pPs) {
  GFraMe_event_update_begin();
    
    // Update everything
    pl_update(pPs->pPl, GFraMe_event_elapsed);
    
    // Collide everything
    pl_collideAgainstGroup(pPs->pPl, pPs->pWalls, pPs->wallsLen,
        0 /*isPlFixed*/, 1/*isObjsFixed*/);
  GFraMe_event_update_end();
}

void ps_draw(struct stPlaystate *pPs) {
  GFraMe_event_draw_begin();
    ps_drawMap(pPs);
    pl_draw(pPs->pPl, pPs->pCam);
  GFraMe_event_draw_end();
}

void ps_clean(struct stPlaystate *pPs) {
    if (pPs->pPl)
        pl_free(&pPs->pPl);
    if (pPs->pCam)
        cam_free(&pPs->pCam);
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
    //    GFraMe_event_on_mouse_up();
    //    GFraMe_event_on_mouse_down();
    //    GFraMe_event_on_mouse_moved();
    //    GFraMe_event_on_finger_down();
    //    GFraMe_event_on_finger_up();
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

