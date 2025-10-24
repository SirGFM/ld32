#include <core/assets.h>
#include <core/core.h>
#include <core/types.h>
#include <collision.h>
#include <error.h>
#include <particles/effects.h>
#include <particles/effects/gem_dust.h>
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
	ASSERT_OK(gemDust_reset(), __ret);

	rv = 0;
__ret:
	return rv;
}


int effects_update(struct scene *scene) {
	int rv = 1;

	ASSERT_OK(gemDust_update(scene), __ret);
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


/* === Mapping of a effects's tile to its animation frame =================== */
#define RED_DUST_1 270
#define RED_DUST_2 330
#define RED_DUST_3 331
#define RED_DUST_4 332
#define RED_DUST_5 333
#define RED_DUST_6 334
#define ORA_DUST_1 398
#define ORA_DUST_2 458
#define ORA_DUST_3 459
#define ORA_DUST_4 460
#define ORA_DUST_5 461
#define ORA_DUST_6 462
#define YEL_DUST_1 526
#define YEL_DUST_2 586
#define YEL_DUST_3 587
#define YEL_DUST_4 588
#define YEL_DUST_5 589
#define YEL_DUST_6 590
#define GRE_DUST_1 654
#define GRE_DUST_2 714
#define GRE_DUST_3 715
#define GRE_DUST_4 716
#define GRE_DUST_5 717
#define GRE_DUST_6 718
#define CYA_DUST_1 782
#define CYA_DUST_2 842
#define CYA_DUST_3 843
#define CYA_DUST_4 844
#define CYA_DUST_5 845
#define CYA_DUST_6 846
#define BLU_DUST_1 910
#define BLU_DUST_2 970
#define BLU_DUST_3 971
#define BLU_DUST_4 972
#define BLU_DUST_5 973
#define BLU_DUST_6 974
#define PUR_DUST_1 1038
#define PUR_DUST_2 1098
#define PUR_DUST_3 1099
#define PUR_DUST_4 1100
#define PUR_DUST_5 1101
#define PUR_DUST_6 1102


/** Animation data for each effect particle. */
static int effectsAnim[] = {
/*                      */ /*len|fps|loop|frames...*/
/* RED_DUST             */    12, 10,  0 , RED_DUST_1,RED_DUST_2,RED_DUST_1,RED_DUST_2,RED_DUST_3,RED_DUST_4,RED_DUST_3,RED_DUST_4,RED_DUST_5,RED_DUST_6,RED_DUST_5,RED_DUST_6,
/* ORANGE_DUST          */    12, 10,  0 , ORA_DUST_1,ORA_DUST_2,ORA_DUST_1,ORA_DUST_2,ORA_DUST_3,ORA_DUST_4,ORA_DUST_3,ORA_DUST_4,ORA_DUST_5,ORA_DUST_6,ORA_DUST_5,ORA_DUST_6,
/* YELLOW_DUST          */    12, 10,  0 , YEL_DUST_1,YEL_DUST_2,YEL_DUST_1,YEL_DUST_2,YEL_DUST_3,YEL_DUST_4,YEL_DUST_3,YEL_DUST_4,YEL_DUST_5,YEL_DUST_6,YEL_DUST_5,YEL_DUST_6,
/* GREEN_DUST           */    12, 10,  0 , GRE_DUST_1,GRE_DUST_2,GRE_DUST_1,GRE_DUST_2,GRE_DUST_3,GRE_DUST_4,GRE_DUST_3,GRE_DUST_4,GRE_DUST_5,GRE_DUST_6,GRE_DUST_5,GRE_DUST_6,
/* CYAN_DUST            */    12, 10,  0 , CYA_DUST_1,CYA_DUST_2,CYA_DUST_1,CYA_DUST_2,CYA_DUST_3,CYA_DUST_4,CYA_DUST_3,CYA_DUST_4,CYA_DUST_5,CYA_DUST_6,CYA_DUST_5,CYA_DUST_6,
/* BLUE_DUST            */    12, 10,  0 , BLU_DUST_1,BLU_DUST_2,BLU_DUST_1,BLU_DUST_2,BLU_DUST_3,BLU_DUST_4,BLU_DUST_3,BLU_DUST_4,BLU_DUST_5,BLU_DUST_6,BLU_DUST_5,BLU_DUST_6,
/* PURPLE_DUST          */    12, 10,  0 , PUR_DUST_1,PUR_DUST_2,PUR_DUST_1,PUR_DUST_2,PUR_DUST_3,PUR_DUST_4,PUR_DUST_3,PUR_DUST_4,PUR_DUST_5,PUR_DUST_6,PUR_DUST_5,PUR_DUST_6,
/* ANIM_MAX_EFFECT_ANIM */    1 , 10,  0 , -1,
};


/** Number entries in the effects animation data. */
static const int effectsAnimLen = sizeof(effectsAnim) / sizeof(int);
