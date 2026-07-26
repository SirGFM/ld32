#ifndef RAINBOW_H
#define RAINBOW_H


#include <core/types.h>
#include <scene.h>

#include <GFraMe/gfmSprite.h>


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


/**
 * rainbow_draw draws every rainbow particle.
 *
 * @return 0: Success; Anything else: failure.
 */
int rainbow_draw(struct scene *scene);


/**
 * rainbow_spawn tries to spawn rainbow particles,
 * respecting the spawner's built-in timer.
 *
 * @param [in] colors: Which colors of the rainbow should be shot.
 * @param [in] dx: The cosine of the particle's shooting direction.
 * @param [in] dy: The sine of the particle's shooting direction.
 * @param [in] cx: The horizontal position of the spawner's center.
 * @param [in] cy: The vertical position of the spawner's center.
 * @param [in] dist: Distance from the spawner's center, at which the particles are spawned.
 * @return 0: Success; Anything else: failure.
 */
int rainbow_spawn(enum rainbowColor colors, double dx, double dy, int cx, int cy, int dist);


/**
 * rainbow_spawnSingleColor tries to spawn rainbow particles,
 * respecting the spawner's built-in timer.
 *
 * @param [in] color: Which color of the rainbow should be shot.
 * @param [in] count: How many particles should be shot.
 * @param [in] dx: The cosine of the particle's shooting direction.
 * @param [in] dy: The sine of the particle's shooting direction.
 * @param [in] cx: The horizontal position of the spawner's center.
 * @param [in] cy: The vertical position of the spawner's center.
 * @param [in] dist: Distance from the spawner's center, at which the particles are spawned.
 * @return 0: Success; Anything else: failure.
 */
int rainbow_spawnSingleColor(
	enum rainbowColor color
	, int count
	, double dx
	, double dy
	, int cx
	, int cy
	, int dist
);


/**
 * rainbow_explodeBullet changes the animation frame to the bullet's explosion.
 *
 * @param [in] bullet: The bullet particle.
 * @return 0: Success; Anything else: failure.
 */
int rainbow_explodeBullet(gfmSprite *bullet);


#endif /* RAINBOW_H */
