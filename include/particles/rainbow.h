#ifndef RAINBOW_H
#define RAINBOW_H


#include <scene.h>

#include <GFraMe/gfmGroup.h>


/**
 * rainbow_init initializes the global rainbow group.
 *
 * @return 0: Success; Anything else: failure.
 */
int rainbow_init();


/**
 * rainbow_free releases all resources associated with the global rainbow group.
 */
void rainbow_free();


/**
 * rainbow_reset despawns every spawned rainbow particle,
 * keeping the particles allocated for further reuse.
 */
int rainbow_reset();


/**
 * rainbow_update updates every rainbow particle.
 *
 * @return 0: Success; Anything else: failure.
 */
int rainbow_update(struct scene *scene);


#endif /* RAINBOW_H */
