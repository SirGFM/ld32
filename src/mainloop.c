#include <core/input.h>
#include <error.h>
#include <scene.h>
#include <mainloop.h>
#include <preload_map.h>


/**
 * STR converts a static/hard-coded string
 * to an actual string + length tuple.
 */
#define STR(VALUE) VALUE, sizeof(VALUE) - 1


/** The currently executing scene. */
static struct scene _curScene = {0};


int mainloop_init() {
	int rv;

	ASSERT_OK(rv = preloadMap_loadAll(), __ret);

	ASSERT_OK(
		rv = scene_loadSceneFromFile(
			&_curScene
			, STR("maps/menus/mainmenu")
		)
		, __ret
	);

__ret:
	return rv;
}


int mainloop_update() {
	int rv;

	ASSERT_OK(rv = input_update(), __ret);
	ASSERT_OK(rv = scene_update(&_curScene), __ret);

__ret:
	return rv;
}


int mainloop_draw() {
	int rv;

	ASSERT_OK(rv = scene_draw(&_curScene), __ret);

__ret:
	return rv;
}


int mainloop_free() {
	int rv;

	ASSERT_OK(rv = scene_free(&_curScene), __ret);
	ASSERT_OK(rv = preloadMap_free(), __ret);

__ret:
	return rv;
}
