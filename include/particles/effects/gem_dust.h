#ifndef GEM_DUST_H
#define GEM_DUST_H


#include <scene.h>


/**
 * gemDust_update handles spawning dust particles.
 *
 * @param [in] scene: The scene being rendered.
 * @return 0: Success; Anything else: failure.
 */
int gemDust_update(struct scene *scene);


/**
 * gemDust_reset resets any dust currently being spawned.
 *
 * @return 0: Success; Anything else: failure.
 */
int gemDust_reset();


#endif /* GEM_DUST_H */
