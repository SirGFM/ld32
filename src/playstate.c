/**
 * @file src/playstate.c
 */
#include <GFraMe/GFraMe_controller.h>
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_event.h>
#include <GFraMe/GFraMe_keys.h>

GFraMe_event_setup();

#include "global.h"
#include "playstate.h"

void ps_event();

GFraMe_ret ps_init() {
    GFraMe_ret rv = GFraMe_ret_ok;

    GFraMe_event_init(UPS, DPS);
    return rv;
}

void ps_update() {
  GFraMe_event_update_begin();

  GFraMe_event_update_end();
}

void ps_draw() {
  GFraMe_event_draw_begin();

  GFraMe_event_draw_end();
}

void ps_clean() {

}

void playstate() {
    GFraMe_ret rv;

    rv = ps_init();
    ASSERT(rv == GFraMe_ret_ok);

    while (gl_running) {
        ps_event();
        ps_update();
        ps_draw();
    }
__ret:
    ps_clean();
    return;
}

void ps_event() {
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

