#ifndef MAINLOOP_H
#define MAINLOOP_H

#include <core/map.h>


/**
 * mainloop_init configures the game's initial state.
 *
 * @return 0: Success; Anything else: failure.
 */
int mainloop_init();


/**
 * mainloop_handleDebug updates the debug buttons.
 *
 * @param [out] canUpdate: Whether the game may be updated.
 * @return 0: Success; Anything else: failure.
 */
int mainloop_handleDebug(int *canUpdate);


/**
 * mainloop_update integrates a single frame.
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


/**
 * mainloop_swapScene queues a scene transition starting on the next frame.
 *
 * If animated is false, the transition happens immediately.
 * Otherwise, a brief animation plays sliding from one scene to the other.
 *
 * @param [in] map: The base map for this new scene.
 * @param [in] animated: Whether the transition animation should play.
 * @return 0: Success; Anything else: failure.
 */
int mainloop_swapScene(struct map *map, int animated);


/**
 * mainloop_finishSwap signals that the transition animation finished.
 *
 * @return 0: Success; Anything else: failure.
 */
int mainloop_finishSwap();


#endif /* MAINLOOP_H */
