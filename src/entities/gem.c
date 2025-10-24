#include <error.h>
#include <core/assets.h>
#include <core/attributes.h>
#include <core/core.h>
#include <core/map.h>
#include <core/types.h>
#include <core/types_bitmask.h>
#include <global.h>
#include <particles/effects.h>
#include <scene.h>
#include <entities/gem.h>
#include <util.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>


/** Time it takes for the trail effect to loop back to the origin. */
#define MAX_TIMER_MS 5000
/** How far can the wave effect can move vertically (in pixels). */
#define VER_WAVE_OFFSET 6.0f


/** Animation data for each gem color. */
static int animationData[];


/** Number entries in the gem animation data. */
static const int animationDataLen;


/**
 * gem_noop implements a no-op handler.
 *
 * @param [in] entity: The gem's embedded entity.
 * @param [in] scene: The scene that called this function.
 * @return 0: Success; Anything else: failure.
 */
static int gem_noop(struct entity *entity, struct scene *scene) {
	return 0;
}


/**
 * gem_preUpdate handles the gem translation.
 *
 * @param [in] entity: The gem's embedded entity.
 * @param [in] scene: The scene that called this function.
 * @return 0: Success; Anything else: failure.
 */
static int gem_preUpdate(struct entity *entity, struct scene *scene) {
	struct gem *gem = (struct gem*)entity;
	int rv = 1;

	if (gem->active) {
		float perc;
		int offY;

		gem->timerMs += scene->elapsedMs;
		if (gem->timerMs >= MAX_TIMER_MS) {
			gem->timerMs -= MAX_TIMER_MS;
		}

		perc = gem->timerMs / (float)MAX_TIMER_MS;
		offY = (int)(VER_WAVE_OFFSET * sin(perc * M_PI * 2.0f));

		ASSERT(GFMRV_OK == gfmSprite_setOffset(gem->base.sprite, 0, offY), __ret);
	}

	rv = 0;
__ret:
	return rv;
}


/**
 * gem_draw draws the gem.
 *
 * @param [in] entity: The gem's embedded entity.
 * @param [in] scene: The scene that called this function.
 * @return 0: Success; Anything else: failure.
 */
static int gem_draw(struct entity *entity, struct scene *scene) {
	struct gem *gem = (struct gem*)entity;
	int rv = 1;

	if (gem->active) {
		ASSERT(GFMRV_OK == gfmSprite_draw(gem->base.sprite, gameCtx), __ret);
	}

	rv = 0;
__ret:
	return rv;
}



/**
 * gem_staticFree releases every resource allocated into a gem,
 * except by the gem's memory itself.
 *
 * Therefore, this is supposed to be called on a stack-allocated gem.
 *
 * @param [in] gem: The gem's embedded entity.
 * @return 0: Success; Anything else: failure.
 */
static int gem_staticFree(struct gem *gem) {
	entity_free(&gem->base);
	return 0;
}


/**
 * gem_free releases every resource allocated by gem_new.
 *
 * @param [in] entity: The gem's embedded entity.
 * @param [in] scene: The scene that called this function.
 * @return 0: Success; Anything else: failure.
 */
static int gem_free(struct entity *entity, struct scene *scene) {
	struct gem *gem = (struct gem*)entity;

	gem_staticFree(gem);

	free(gem);
	return 0;
}


int gem_new(struct entity **entity, struct mapObject *data) {
	struct entity *ret = 0;
	struct gem tmp = {0};
	int rv = 1;
	int color;

	ASSERT_OK(
		entity_init(
			&tmp.base
			, data->x
			, data->y
			, data->width
			, data->height
			, gfx16x16
			, data->type
		)
		, __ret
	);

	/* Check if the stone has been retrieved. */
	color = GET_EXTRA_TYPE_DATA(data->type);
	tmp.active = 0 == (global_getPermanent().stones & color);

	/* Load animations */
	if (tmp.active) {
		ASSERT(
			GFMRV_OK == gfmSprite_addAnimations(
				tmp.base.sprite
				, animationData
				, animationDataLen
			)
			, __ret
		);
		ASSERT(GFMRV_OK == gfmSprite_playAnimation(tmp.base.sprite, color - 1), __ret);
	}

	/* Configure the vtable. */
	tmp.base.fn.preUpdate = gem_preUpdate;
	tmp.base.fn.postUpdate = gem_noop;
	tmp.base.fn.draw = gem_draw;
	tmp.base.fn.free = gem_free;

	/* Return the initialized object. */
	ASSERT((ret = malloc(sizeof(tmp))) != 0, __ret);
	memcpy(ret, &tmp, sizeof(tmp));
	memset(&tmp, 0x0, sizeof(tmp));

	*entity = ret;
	ret = 0;

	rv = 0;
__ret:
	if (ret) {
		free(ret);
	}

	gem_staticFree(&tmp);
	return rv;
}


int gem_isActive(struct entity *entity) {
	struct gem *gem = (struct gem*)entity;
	return gem->active;
}


int gem_get(struct entity *entity) {
	struct gem *gem = (struct gem*)entity;
	int rv = 1;

	if (gem->active) {
		int w, h, ox, oy, x, y;
		enum rainbowColor color;

		switch (gem->base.type) {
		case TYP_GEM_RED:
			global_addRedStone();
			break;
		case TYP_GEM_ORANGE:
			global_addOrangeStone();
			break;
		case TYP_GEM_YELLOW:
			global_addYellowStone();
			break;
		case TYP_GEM_GREEN:
			global_addGreenStone();
			break;
		case TYP_GEM_CYAN:
			global_addCyanStone();
			break;
		case TYP_GEM_BLUE:
			global_addBlueStone();
			break;
		case TYP_GEM_PURPLE:
			global_addPurpleStone();
			break;
		}

		/* Spawn particles. */
		color = GET_EXTRA_TYPE_DATA(gem->base.type);

		ASSERT(GFMRV_OK == gfmSprite_getPosition(&x, &y, gem->base.sprite), __ret);
		ASSERT(GFMRV_OK == gfmSprite_getOffset(&ox, &oy, gem->base.sprite), __ret);
		ASSERT(GFMRV_OK == gfmSprite_getDimensions(&w, &h, gem->base.sprite), __ret);

		x += w / 2 + ox;
		y += h / 2 + oy;

		ASSERT_OK(effects_spawnGemDust(color, x, y), __ret);
	}
	gem->active = 0;

	rv = 0;
__ret:
	return rv;
}


/* === Mapping of a gem's tile to its animation frame =================== */
#define RED_1 64
#define RED_2 65
#define RED_3 66
#define RED_4 67
#define RED_5 68
#define ORA_1 96
#define ORA_2 97
#define ORA_3 98
#define ORA_4 99
#define ORA_5 100
#define YEL_1 128
#define YEL_2 129
#define YEL_3 130
#define YEL_4 131
#define YEL_5 132
#define GRE_1 160
#define GRE_2 161
#define GRE_3 162
#define GRE_4 163
#define GRE_5 164
#define CYA_1 192
#define CYA_2 193
#define CYA_3 194
#define CYA_4 195
#define CYA_5 196
#define BLU_1 224
#define BLU_2 225
#define BLU_3 226
#define BLU_4 227
#define BLU_5 228
#define PUR_1 256
#define PUR_2 257
#define PUR_3 258
#define PUR_4 259
#define PUR_5 260


/** Animation data for each gem color. */
static int animationData[] = {
/*                   */ /*len|fps|loop|frames...*/
/*     RED_BULLET    */    40, 12,  1 , RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_1,RED_2,RED_3,RED_4,RED_5,
/*   ORANGE_BULLET   */    40, 12,  1 , ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_1,ORA_2,ORA_3,ORA_4,ORA_5,
/*   YELLOW_BULLET   */    40, 12,  1 , YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_1,YEL_2,YEL_3,YEL_4,YEL_5,
/*    GREEN_BULLET   */    40, 12,  1 , GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_1,GRE_2,GRE_3,GRE_4,GRE_5,
/*    CYAN_BULLET    */    40, 12,  1 , CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_1,CYA_2,CYA_3,CYA_4,CYA_5,
/*    BLUE_BULLET    */    40, 12,  1 , BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_1,BLU_2,BLU_3,BLU_4,BLU_5,
/*   PURPLE_BULLET   */    40, 12,  1 , PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_1,PUR_2,PUR_3,PUR_4,PUR_5,
};


/** Number entries in the gem animation data. */
static const int animationDataLen = sizeof(animationData) / sizeof(int);
