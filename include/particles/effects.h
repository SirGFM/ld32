#ifndef EFFECTS_H
#define EFFECTS_H


#include <core/types.h>
#include <scene.h>


enum effectsAnimation {
	  ANIM_MAX_EFFECT_ANIM = 0
};


/**
 * effects_init initializes the global effects group.
 *
 * @return 0: Success; Anything else: failure.
 */
int effects_init();


/**
 * effects_free releases all resources associated with the global effects group.
 */
void effects_free();


/**
 * effects_reset despawns every spawned effect particle,
 * keeping the particles allocated for further reuse.
 */
int effects_reset();


/**
 * effects_update updates every effect particle.
 *
 * @return 0: Success; Anything else: failure.
 */
int effects_update(struct scene *scene);


/**
 * effects_draw draws every effect particle.
 *
 * @return 0: Success; Anything else: failure.
 */
int effects_draw(struct scene *scene);


#endif /* EFFECTS_H */
