#ifndef EFFECTS_H
#define EFFECTS_H


#include <core/types.h>
#include <scene.h>


enum effectsAnimation {
	  ANIM_RED_DUST = 0
	, ANIM_ORANGE_DUST
	, ANIM_YELLOW_DUST
	, ANIM_GREEN_DUST
	, ANIM_CYAN_DUST
	, ANIM_BLUE_DUST
	, ANIM_PURPLE_DUST
	, ANIM_MAX_EFFECT_ANIM
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


/**
 * effects_spawnGemDust spawns colored dust after a gem is collected.
 *
 * @param [in] colors: Which colors of the rainbow should be shot.
 * @param [in] x: The center horizontal position from which particles will spawn.
 * @param [in] y: The center horizontal position from which particles will spawn.
 * @return 0: Success; Anything else: failure.
 */
int effects_spawnGemDust(enum rainbowColor color, int x, int y);


#endif /* EFFECTS_H */
