#include <core/assets.h>
#include <core/core.h>
#include <core/types.h>
#include <collision.h>
#include <error.h>
#include <particles/effects.h>
#include <scene.h>
#include <util.h>

#include <GFraMe/gfmGroup.h>
#include <GFraMe/gfmSprite.h>

#include <stdint.h>


/** Maximum number of effect particles on screen at once. */
#define MAX_EFFECTS 1000

/** For how long each effect particle should live by default. */
#define MAX_TTL_MS 5000


/** Effects particles, used by anything and everything. */
static gfmGroup *effects = 0;


/** Animation data for each effect particle. */
static int effectsAnim[];


/** Number entries in the effects animation data. */
static const int effectsAnimLen;


/** Data for each animation. */
static int *effectsAnimationData[ANIM_MAX_EFFECT_ANIM] = {};


int effects_init() {
	static gfmGroup *tmpEffects = 0;
	int rv = 1;
	int i;

	/* Alloc the particles and set its default attributes. */
	ASSERT(GFMRV_OK == gfmGroup_getNew(&tmpEffects), __ret);
	ASSERT(GFMRV_OK == gfmGroup_setDefSpriteset(tmpEffects, gfx8x8), __ret);
	ASSERT(GFMRV_OK == gfmGroup_setDefDimensions(tmpEffects, 4, 4, -2, -2), __ret);
	ASSERT(
		GFMRV_OK == gfmGroup_setDefAnimData(
			tmpEffects
			, effectsAnim
			, effectsAnimLen
		)
		, __ret
	);
	ASSERT(GFMRV_OK ==  gfmGroup_setDeathOnLeave(tmpEffects, 0), __ret);
	ASSERT(GFMRV_OK == gfmGroup_setDeathOnTime(tmpEffects, MAX_TTL_MS), __ret);

	/* Pre-cache some sprites.
	 * TODO: Make this configurable.
	 */
	ASSERT(
		GFMRV_OK == gfmGroup_preCache(
			tmpEffects
			, MAX_EFFECTS
			, MAX_EFFECTS
		)
		, __ret
	);
	ASSERT(
		GFMRV_OK == gfmGroup_setDrawOrder(tmpEffects, gfmDrawOrder_newestFirst)
		, __ret
	);

	effects = tmpEffects;
	tmpEffects = 0;

	/* Initialize the animation data helper. */
	effectsAnimationData[0] = effectsAnim;
	for (i = 1; i < ANIM_MAX_EFFECT_ANIM; i++) {
		int len = effectsAnimationData[i - 1][0];
		effectsAnimationData[i] = effectsAnimationData[i - 1] + len + 3;
	}

	rv = 0;
__ret:
	if (tmpEffects) {
		gfmGroup_free(&tmpEffects);
	}

	return rv;
}


void effects_free() {
	if (effects) {
		gfmGroup_free(&effects);
		effects = 0;
	}
}


int effects_reset() {
	int rv = 1;

	ASSERT(GFMRV_OK == gfmGroup_killAll(effects), __ret);

	rv = 0;
__ret:
	return rv;
}


int effects_update(struct scene *scene) {
	int rv = 1;

	ASSERT(GFMRV_OK == gfmGroup_update(effects, gameCtx), __ret);

	rv = 0;
__ret:
	return rv;
}


int effects_draw(struct scene *scene) {
	int rv = 1;

	ASSERT(GFMRV_OK == gfmGroup_draw(effects, gameCtx), __ret);

	rv = 0;
__ret:
	return rv;
}


/**
 * effects_getGroup retrieves the effects particles group.
 *
 * This must be manually referenced in files that want to use it.
 *
 * @return The particle group.
 */
gfmGroup* effects_getGroup() {
	return effects;
}


/** Animation data for each effect particle. */
static int effectsAnim[] = {
/*                      */ /*len|fps|loop|frames...*/
/* ANIM_MAX_EFFECT_ANIM */    1 , 10,  0 , -1,
};


/** Number entries in the effects animation data. */
static const int effectsAnimLen = sizeof(effectsAnim) / sizeof(int);
