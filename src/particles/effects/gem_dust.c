#include <core/types.h>
#include <error.h>
#include <game_math.h>
#include <particles/effects.h>
#include <particles/effects/gem_dust.h>
#include <scene.h>
#include <util.h>

#include <GFraMe/gfmGroup.h>


extern gfmGroup* effects_getGroup();


/** Number of waves that explode before stopping the effect. */
#define NUM_WAVES 4

/** Delay, in milliseconds, between explosions. */
#define WAVE_DELAY_MS FRAMES_TO_MS(28)

/** How long each particle is visible. */
#define DUST_TTL_MS FRAMES_TO_MS(72)

/** The absolute speed of the particles. */
#define DUST_SPEED SPEED(120, 9)


/** How many waves must still be spawned. */
static int count = 0;

/** Delay between waves. */
static int timer = 0;

/** Starting horizontal position from which the particles shall spawn. */
static int cx = 0;

/** Starting vertical position from which the particles shall spawn. */
static int cy = 0;

/** The animation used by this effect. */
static enum effectsAnimation anim = 0;


/**
 * gemDust_spawn spawns a single particle in the effects group.
 *
 * @param [in] vx: The horizontal speed.
 * @param [in] vy: The vertical speed.
 * @return 0: Success; Anything else: failure.
 */
static int gemDust_spawn(double vx, double vy) {
	void *ptr;
	gfmSprite *tmp;
	gfmGroupNode *node;
	gfmGroup *group = effects_getGroup();
	int type;
	int rv = 1;

	ASSERT(GFMRV_OK == gfmGroup_recycle(&tmp, group), __ret);
	ASSERT(GFMRV_OK == gfmGroup_setPosition(group, cx, cy), __ret);
	ASSERT(GFMRV_OK == gfmGroup_setAnimation(group, anim), __ret);
	ASSERT(GFMRV_OK == gfmGroup_setVelocity(group, vx, vy), __ret);

	ASSERT(GFMRV_OK == gfmSprite_getChild(&ptr, &type, tmp), __ret);
	node = (gfmGroupNode*)ptr;
	ASSERT(GFMRV_OK == gfmGroup_setTimeToLive(node, DUST_TTL_MS), __ret);

	rv = 0;
__ret:
	return rv;
}


int gemDust_update(struct scene *scene) {
	int rv = 1;

	if (count == 0) {
		return 0;
	}

	if (timer > 0) {
		timer -= scene->elapsedMs;
	}

	if (timer <= 0) {
		double vx = 1.0;
		double vy = 1.0;

		normalize(&vx, &vy);
		vx *= DUST_SPEED;
		vy = vx;

		ASSERT_OK(gemDust_spawn(DUST_SPEED, 0), __ret);
		ASSERT_OK(gemDust_spawn(vx, vy), __ret);
		ASSERT_OK(gemDust_spawn(0, DUST_SPEED), __ret);
		ASSERT_OK(gemDust_spawn(-vx, vy), __ret);
		ASSERT_OK(gemDust_spawn(-DUST_SPEED, 0), __ret);
		ASSERT_OK(gemDust_spawn(-vx, -vy), __ret);
		ASSERT_OK(gemDust_spawn(0, -DUST_SPEED), __ret);
		ASSERT_OK(gemDust_spawn(vx, -vy), __ret);

		timer += WAVE_DELAY_MS;
		count--;
	}

	rv = 0;
__ret:
	return rv;
}


int gemDust_reset() {
	timer = 0;
	count = 0;

	return 0;
}


int effects_spawnGemDust(enum rainbowColor color, int x, int y) {
	count = NUM_WAVES;
	timer = 0;
	cx = x - 2;
	cy = y - 2;

	switch (color) {
	case RED_COLOR:
		anim = ANIM_RED_DUST;
		break;
	case ORANGE_COLOR:
		anim = ANIM_ORANGE_DUST;
		break;
	case YELLOW_COLOR:
		anim = ANIM_YELLOW_DUST;
		break;
	case GREEN_COLOR:
		anim = ANIM_GREEN_DUST;
		break;
	case CYAN_COLOR:
		anim = ANIM_CYAN_DUST;
		break;
	case BLUE_COLOR:
		anim = ANIM_BLUE_DUST;
		break;
	case PURPLE_COLOR:
		anim = ANIM_PURPLE_DUST;
		break;
	}

	return 0;
}
