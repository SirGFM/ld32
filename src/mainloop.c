#include <camera.h>
#include <core/input.h>
#include <error.h>
#include <scene.h>
#include <mainloop.h>
#include <preload_map.h>

#include <string.h>


/**
 * STR converts a static/hard-coded string
 * to an actual string + length tuple.
 */
#define STR(VALUE) VALUE, sizeof(VALUE) - 1


/**
 * How long an animated scene transition should take,
 * in milliseconds.
 */
#define TRANSITION_DELAY_MS 2500


enum transition {
	TRANSITION_NONE = 0
	, TRANSITION_IMMEDIATE
	, TRANSITION_PRE_SLIDE
	, TRANSITION_SLIDING
	, TRANSITION_DONE
	, TRANSITION_MAX
};


enum debugMode {
	DEBUG_RUNNING = 0
	, DEBUG_PAUSED
	, DEBUG_STEP
};


/** The currently executing scene. */
static struct scene _curScene = {0};

/** The scene being transitioned to. */
static struct scene _nextScene = {0};

/** The transition state of the game. */
static enum transition _state = TRANSITION_NONE;

/** The game's debug mode. */
static enum debugMode _mode = DEBUG_RUNNING;


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


int mainloop_handleDebug(int *canUpdate) {
	int rv = 1;

#if defined(DEBUG)
	/* Forcefully update the debug keys. */
	ASSERT_OK(input_updateDebug(), __ret);

	if (input_isJustPressed(DEBUG_INPUT_QT)) {
		scene_flipCollisionVisibility(&_curScene);
	}

	if (input_isJustPressed(DEBUG_INPUT_PAUSE)) {
		if (_mode == DEBUG_PAUSED) {
			_mode = DEBUG_RUNNING;
		}
		else {
			_mode = DEBUG_PAUSED;
		}
	}

	if (input_isJustPressed(DEBUG_INPUT_STEP)) {
		_mode = DEBUG_STEP;
	}

	*canUpdate = (_mode != DEBUG_PAUSED);
#endif /* defined(DEBUG) */

	rv = 0;
__ret:
	return rv;
}


/**
 * mainloop_finishTransition moves the new scene back to the origin
 * and adjusts the camera to its new position in world space
 * (but the same position in "scene-space").
 *
 * @return 0: Success; Anything else: failure.
 */
static int mainloop_finishTransition() {
	int rv;
	int offsetX, offsetY;

	/* Calculate the distance between the current scene,
	 * whose origin should be on (0, 0),
	 * and the next scene. */
	ASSERT_OK(
		rv = scene_getOffset(
			&offsetX
			, &offsetY
			, &_nextScene
			, &_curScene
		)
		, __ret
	);

	/* Move the camera to match the scene's new position. */
	ASSERT_OK(rv = camera_translate(offsetX, offsetY), __ret);

	/* Adjust character's position. */
	ASSERT_OK(rv = scene_finishPlayerSlide(&_curScene, &_nextScene), __ret);

	/* Move the next scene back to (0, 0). */
	ASSERT_OK(
		rv = scene_setRelativePosition(
			&_nextScene
			, &_nextScene
		)
		, __ret
	);

__ret:
	return rv;
}


int mainloop_update() {
	int rv;

	if (_state == TRANSITION_IMMEDIATE || _state == TRANSITION_DONE) {
		ASSERT_OK(rv = mainloop_finishTransition(), __ret);
		ASSERT_OK(rv = scene_free(&_curScene), __ret);
		memcpy(&_curScene, &_nextScene, sizeof(_nextScene));
		memset(&_nextScene, 0, sizeof(_nextScene));

		_state = TRANSITION_NONE;
	}

	ASSERT_OK(rv = input_update(), __ret);

	if (_state == TRANSITION_PRE_SLIDE || _state == TRANSITION_SLIDING) {
		_state = TRANSITION_SLIDING;

		ASSERT_OK(rv = scene_updateTransition(&_curScene), __ret);
		ASSERT_OK(rv = camera_update(), __ret);

		if (!camera_isMoving()) {
			_state = TRANSITION_DONE;
		}
	}
	else {
		ASSERT_OK(rv = scene_update(&_curScene), __ret);
	}

#if defined(DEBUG)
	if (_mode == DEBUG_STEP) {
		_mode = DEBUG_PAUSED;
	}
#endif /* defined(DEBUG) */

__ret:
	return rv;
}


int mainloop_draw() {
	int rv;

	/* For some reason, the new scene would be slightly offset
	 * on the first frame after setting its position.
	 * So, PRE_SLIDING is ignored here. */
	if (_state == TRANSITION_SLIDING || _state == TRANSITION_DONE) {
		ASSERT_OK(rv = scene_draw(&_nextScene, 0), __ret);
	}

	ASSERT_OK(rv = scene_draw(&_curScene, 1), __ret);

__ret:
	return rv;
}


int mainloop_free() {
	int rv;

	ASSERT_OK(rv = scene_free(&_nextScene), __ret);
	ASSERT_OK(rv = scene_free(&_curScene), __ret);
	ASSERT_OK(rv = preloadMap_free(), __ret);

__ret:
	return rv;
}


/**
 * mainloop_swapScene queues a scene transition starting on the next frame.
 *
 * If animated is false, the transition happens immediately.
 * Otherwise, a brief animation plays sliding from one scene to the other.
 * In that case, doorID is used to calculate final camera position
 * relative to the this door in the new scene.
 *
 * @param [in] map: The base map for this new scene.
 * @param [in] animated: Whether the transition animation should play.
 * @param [in] doorID: Unique value that identifies the same door in two different scenes.
 * @return 0: Success; Anything else: failure.
 */
static int mainloop_swapScene(struct map *map, int animated, int doorID) {
	struct scene tmp = {0};
	int rv = 1;

	/* Allow overwriting only animated transitions. */
	ASSERT(_state != TRANSITION_SLIDING, __ret);
	if (_state != TRANSITION_NONE) {
		ASSERT_OK(scene_free(&_nextScene), __ret);
	}

	ASSERT_OK(scene_loadScene(&tmp, map), __ret);

	if (animated) {
		int x, y;

		_state = TRANSITION_PRE_SLIDE;

		ASSERT_OK(scene_setRelativePosition(&tmp, &_curScene), __ret);
		ASSERT_OK(
			scene_getCameraTransitionPosition(
				&x
				, &y
				, &tmp
				, &_curScene
				, doorID
			)
			, __ret
		);
		ASSERT_OK(camera_moveToPosition(x, y, TRANSITION_DELAY_MS), __ret);

		ASSERT_OK(
			scene_setupPlayerSlide(
				&_curScene
				, &tmp
				, doorID
				, TRANSITION_DELAY_MS
			)
			, __ret
		);
	}
	else {
		_state = TRANSITION_IMMEDIATE;
	}

	memcpy(&_nextScene, &tmp, sizeof(tmp));
	memset(&tmp, 0, sizeof(tmp));

	rv = 0;
__ret:
	scene_free(&tmp);

	return rv;
}


int mainloop_staticSwapScene(struct map *map) {
	return mainloop_swapScene(map, 0, 0);
}


int mainloop_transitionSwapScene(struct map *map, int doorID) {
	return mainloop_swapScene(map, 1, doorID);
}


int mainloop_finishSwap() {
	_state = TRANSITION_DONE;

	return 0;
}
