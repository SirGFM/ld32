#ifndef PARTICLES_H
#define PARTICLES_H


#include <scene.h>


/**
 * particles_init initializes the global particles subsystem.
 *
 * @return 0: Success; Anything else: failure.
 */
int particles_init();


/**
 * particles_free releases all resources associated with the global particles subsystem.
 */
void particles_free();


/**
 * particles_reset despawns every spawned particles,
 * keeping the particles allocated for further reuse.
 *
 * @return 0: Success; Anything else: failure.
 */
int particles_reset();


/**
 * particles_update updates every particle.
 *
 * @return 0: Success; Anything else: failure.
 */
int particles_update(struct scene *scene);


#endif /* PARTICLES_H */
