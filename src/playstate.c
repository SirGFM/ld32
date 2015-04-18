/**
 * @file src/playstate.c
 */
#include <GFraMe/GFraMe_controller.h>
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_event.h>
#include <GFraMe/GFraMe_keys.h>
#include <GFraMe/GFraMe_object.h>

GFraMe_event_setup();

#include "global.h"
#include "player.h"
#include "playstate.h"
#include "map001.h"

struct stPlaystate {
    player *pPl;
    GFraMe_object *pWalls;
    int wallsLen;
};

void ps_event(struct stPlaystate *pPs);

int ps_init(struct stPlaystate *pPs) {
    int rv;
    
    // Initialize everything with 0
    pPs->pPl = 0;
    pPs->pWalls = 0;
    pPs->wallsLen = 0;
    
    // Initialize the player
    pl_getNew(&pPs->pPl);
    ASSERT(pPs->pPl, 1);
    
    map001_getWalls(&(pPs->pWalls), &(pPs->wallsLen));
    
    // Initialize the timer
    GFraMe_event_init(UPS, DPS);
    
    rv = 0;
__ret:
    return rv;
}

void ps_update(struct stPlaystate *pPs) {
  GFraMe_event_update_begin();

  GFraMe_event_update_end();
}

void ps_draw(struct stPlaystate *pPs) {
  GFraMe_event_draw_begin();

  GFraMe_event_draw_end();
}

void ps_clean(struct stPlaystate *pPs) {
    if (pPs->pPl)
        pl_free(&pPs->pPl);
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
#endif
    GFraMe_event_on_key_down();
#ifdef DEBUG
      if (GFraMe_keys.esc)
        gl_running = 0;
#endif
    GFraMe_event_on_key_up();
    GFraMe_event_on_quit();
      gl_running = 0;
    GFraMe_event_end();
}

