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
 * mainloop_staticSwapScene queues a scene transition immediately on the next frame.
 *
 * @param [in] map: The base map for this new scene.
 * @return 0: Success; Anything else: failure.
 */
int mainloop_staticSwapScene(struct map *map);


/**
 * mainloop_transitionSwapScene queues a scene transition starting on the next frame.
 *
 * A brief animation plays sliding from one scene to the other,
 * using doorID to calculate final camera position
 * relative to the this door in the new scene.
 *
 * @param [in] map: The base map for this new scene.
 * @param [in] doorID: Unique value that identifies the same door in two different scenes.
 * @return 0: Success; Anything else: failure.
 */
int mainloop_transitionSwapScene(struct map *map, int doorID);


/**
 * mainloop_finishSwap signals that the transition animation finished.
 *
 * @return 0: Success; Anything else: failure.
 */
int mainloop_finishSwap();


#endif /* MAINLOOP_H */
