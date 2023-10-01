#ifndef MAINLOOP_H
#define MAINLOOP_H


/**
 * mainloop_init configures the game's initial state.
 *
 * @return 0: Success; Anything else: failure.
 */
int mainloop_init();


/**
 *  mainloop_update integrates a single frame.
 *
 * @return 0: Success; Anything else: failure.
 */
int mainloop_update();


/**
 * mainloop_draw renders a single frame.
 *
 * @return 0: Success; Anything else: failure.
 */
int mainloop_draw();


/**
 * mainloop_free releases all resources associated with the game.
 *
 * @return 0: Success; Anything else: failure.
 */
int mainloop_free();


#endif /* MAINLOOP_H */
