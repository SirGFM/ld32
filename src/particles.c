#include <error.h>
#include <particles.h>
#include <particles/rainbow.h>
#include <scene.h>


int particles_init() {
	int rv = 1;

	ASSERT_OK(rainbow_init(), __ret);

	rv = 0;
__ret:

	return rv;
}


void particles_free() {
	rainbow_free();
}


int particles_reset() {
	int rv = 1;

	ASSERT_OK(rainbow_reset(), __ret);

	rv = 0;
__ret:
	return rv;
}


int particles_update(struct scene *scene) {
	int rv = 1;

	ASSERT_OK(rainbow_update(scene), __ret);

	rv = 0;
__ret:
	return rv;
}
