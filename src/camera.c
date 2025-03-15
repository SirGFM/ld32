#include <camera.h>
#include <core/core.h>

#include <GFraMe/gframe.h>
#include <GFraMe/gfmCamera.h>
#include <error.h>

#include <string.h>


struct motion {
	/** Target horizontal position of the camera. */
	int x;
	/** Target vertical position of the camera. */
	int y;
	/** Horizontal speed, in pixels per millisecond. */
	float vx;
	/** Vertical speed, in pixels per millisecond. */
	float vy;
};


struct camera {
	/** The internal camera. */
	gfmCamera *self;
	/** Horizontal position of the camera. */
	float x;
	/** Vertical position of the camera. */
	float y;
	/** A motion torward a specific position. */
	struct motion motion;
};


/** Reference to the game's camera. */
static struct camera camera;


int camera_init() {
	gfmRV grv = GFMRV_OK;

	memset(&camera, 0x0, sizeof(struct camera));
	ASSERT_OK(grv = gfm_getCamera(&camera.self, gameCtx), __ret);

__ret:
	return grv;
}


int camera_update() {
	int elapsedMs;
	int moved = 0;
	gfmRV grv = GFMRV_OK;

	ASSERT_OK(grv = gfm_getElapsedTime(&elapsedMs, gameCtx), __ret);

	/* Use simple Euler integration for the movement. */
	if (camera.motion.vx != 0.0f) {
		camera.x += camera.motion.vx * (float)elapsedMs;

		if (
			(camera.motion.vx > 0 && camera.x > camera.motion.x) ||
			(camera.motion.vx < 0 && camera.x < camera.motion.x)
		) {
			camera.x = (float)camera.motion.x;
			camera.motion.vx = 0.0f;
		}

		moved = 1;
	}
	if (camera.motion.vy != 0.0f) {
		camera.y += camera.motion.vy * (float)elapsedMs;

		if (
			(camera.motion.vy > 0 && camera.y > camera.motion.y) ||
			(camera.motion.vy < 0 && camera.y < camera.motion.y)
		) {
			camera.y = (float)camera.motion.y;
			camera.motion.vy = 0.0f;
		}

		moved = 1;
	}

	if (moved) {
		ASSERT_OK(
			grv = gfmCamera_setPositionUnrestricted(
				camera.self
				, (int)camera.x
				, (int)camera.y
			)
			, __ret
		);
	}

__ret:
	return grv;
}


int camera_setPosition(int x, int y) {
	gfmRV grv = GFMRV_OK;

	ASSERT_OK(grv = gfmCamera_setPositionUnrestricted(camera.self, x, y), __ret);
	camera.x = (float)x;
	camera.y = (float)y;

__ret:
	return grv;
}


int camera_translate(int dx, int dy) {
	gfmRV grv = GFMRV_OK;

	camera.x += (float)dx;
	camera.y += (float)dy;

	ASSERT_OK(
		grv = gfmCamera_setPositionUnrestricted(
			camera.self
			, (int)camera.x
			, (int)camera.y
		)
		, __ret
	);

__ret:
	return grv;
}


int camera_moveToPosition(int x, int y, int durationMs) {
	int srcX, srcY;
	gfmRV grv = GFMRV_OK;

	ASSERT_OK(grv = gfmCamera_getPosition(&srcX, &srcY, camera.self), __ret);
	camera.x = (float)srcX;
	camera.y = (float)srcY;

	camera.motion.x = x;
	camera.motion.y = y;
	camera.motion.vx = (float)(x - srcX) / (float)(durationMs);
	camera.motion.vy = (float)(y - srcY) / (float)(durationMs);

__ret:
	return grv;
}


int camera_isMoving() {
	return camera.motion.vx != 0.0f || camera.motion.vy != 0.0f;
}
