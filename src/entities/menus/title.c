#include <error.h>
#include <core/assets.h>
#include <core/core.h>
#include <core/types.h>
#include <core/map.h>
#include <core/store.h>
#include <scene.h>
#include <entities/menus/title.h>

#include <GFraMe/gfmSprite.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>


/** Time it takes for the trail effect to loop back to the origin. */
#define MAX_TIMER_MS 2500
/** How far can the wave effect can move horizontally (in pixels). */
#define HOR_WAVE_OFFSET 16.0f
/** Horizontal wave multiplier (to offset the horizontal and vertical components). */
#define HOR_WAVE_MULT 1.0f
/** How far can the wave effect can move vertically (in pixels). */
#define VER_WAVE_OFFSET 8.0f
/** Vertical wave multiplier (to offset the horizontal and vertical components). */
#define VER_WAVE_MULT 2.0f
/** Offset, in milliseconds, between each element in the trail. */
#define TRAIL_DELAY_MS 80


/**
 * title_preUpdate moves a rainbow tile around.
 *
 * @param [in] entity: The title's embedded entity.
 * @param [in] scene: The scene that called this function.
 * @return 0: Success; Anything else: failure.
 */
static int title_preUpdate(struct entity *entity, struct scene *scene) {
	struct title *title = (struct title*)entity;
	int i;

	title->timerMs += scene->elapsedMs;
	if (title->timerMs >= MAX_TIMER_MS) {
		title->timerMs -= MAX_TIMER_MS;
	}
	for (i = 0; i < TITLE_NUM_TRAIL; i++) {
		title->trailTimerMs[i] += scene->elapsedMs;
		if (title->trailTimerMs[i] >= MAX_TIMER_MS) {
			title->trailTimerMs[i] -= MAX_TIMER_MS;
		}
	}

	return 0;
}


/**
 * title_getWavePosition calculates the letter's position within the wave.
 *
 * @param [out] x: The letter's new horizontal position.
 * @param [out] y: The letter's new vertical position.
 * @param [in] timerMs: Timer used by the wave effect.
 * @param [in] ox: The letter's original horizontal position.
 * @param [in] oy: The letter's original vertical position.
 */
static void title_getWavePosition(int *x, int *y, int timerMs, int ox, int oy) {
	float perc;

	/* How far along the wave this letter is. */
	perc = timerMs / (float)MAX_TIMER_MS;

	/* Calculate the new position in the wave. */
	*x = ox + (int)(HOR_WAVE_OFFSET * cos(perc * M_PI * 2.0f * HOR_WAVE_MULT));
	*y = oy + (int)(VER_WAVE_OFFSET * sin(perc * M_PI * 2.0f * VER_WAVE_MULT));
}


/**
 * title_draw draws the tile, drawing a trail effect for rainbow tiles.
 *
 * @param [in] entity: The title's embedded entity.
 * @param [in] scene: The scene that called this function.
 * @return 0: Success; Anything else: failure.
 */
static int title_draw(struct entity *entity, struct scene *scene) {
	struct title *title = (struct title*)entity;
	int rv = 1;

	if (title->isRainbow) {
		int i;
		int x;
		int y;

		/* Draw the trail component of the wave. */
		for (i = 0; i < TITLE_NUM_TRAIL; i++) {
			int timer = title->trailTimerMs[TITLE_NUM_TRAIL - i - 1];

			if (timer < 0) {
				continue;
			}

			title_getWavePosition(&x, &y, timer, title->ox, title->oy);
			ASSERT_OK(gfmSprite_setPosition(title->base.sprite, x, y), __ret);
			ASSERT_OK(gfmSprite_draw(title->base.sprite, gameCtx), __ret);
		}

		/** Update the first element. */
		title_getWavePosition(&x, &y, title->timerMs, title->ox, title->oy);
		ASSERT_OK(gfmSprite_setPosition(title->base.sprite, x, y), __ret);
	}

	ASSERT_OK(gfmSprite_draw(title->base.sprite, gameCtx), __ret);

	rv = 0;
__ret:
	return rv;
}


/**
 * title_free releases every resource allocated by title_new.
 *
 * @param [in] entity: The title's embedded entity.
 * @param [in] scene: The scene that called this function.
 * @return 0: Success; Anything else: failure.
 */
static int title_free(struct entity *entity, struct scene *scene) {
	struct title *title = (struct title*)entity;

	entity_free(&title->base);
	free(title);
	return 0;
}


/**
 * title_isRainbowTile checks if the rainbow stone
 * associated with the given character
 * was gotten in any save file.
 *
 * @param [in] c: The lower-case character.
 * @return 1: True, 0: False.
 */
static int title_isRainbowTile(char c) {
	/* TODO: Get if rainbow is enable from the global save file. */
	return 1;
}


/**
 * title_getTile retrieves the spriteset tile associated with the given data.
 *
 * @param [out] tile: The tile.
 * @param [out] isRainbow: Whether this tile is using the rainbow effect.
 * @param [in] data: The title's configuration.
 * @return 0: Success; Anything else: failure.
 */
static int title_getTile(int *tile, int *isRainbow, struct mapObject *data) {
	char *str;
	int len;
	int rv = 1;

	ASSERT(data->numAttribute == 1, __ret);
	ASSERT(
		data->attributes[0].type == ATTR_MAINMENU_LETTER
			|| data->attributes[0].type == ATTR_MAINMENU_RAINBOW
		, __ret
	);

	ASSERT_OK(store_getStr(&str, &len, data->attributes[0].value), __ret);
	ASSERT(len >= 1, __ret);

	*isRainbow = (
		data->attributes[0].type == ATTR_MAINMENU_RAINBOW
		&& title_isRainbowTile(str[0])
	);

	switch (str[0]) {
	case 'u':
		*tile = 186;
		break;
	case 'm':
		*tile = 187;
		break;
	case 'b':
		if (*isRainbow) {
			*tile = 234;
		}
		else {
			*tile = 188;
		}
		break;
	case 'r':
		if (*isRainbow) {
			*tile = 202;
		}
		else {
			*tile = 189;
		}
		break;
	case 'a':
		if (*isRainbow) {
			*tile = 203;
		}
		else {
			*tile = 190;
		}
		break;
	case '\'':
		*tile = 191;
		break;
	case 'i':
		if (*isRainbow) {
			*tile = 218;
		}
		else {
			*tile = 204;
		}
		break;
	case 'n':
		if (*isRainbow) {
			*tile = 219;
		}
		else {
			*tile = 205;
		}
		break;
	case 'o':
		if (*isRainbow) {
			*tile = 235;
		}
		else {
			*tile = 206;
		}
		break;
	case 'w':
		if (*isRainbow) {
			*tile = 236;
		}
		else {
			*tile = 207;
		}
		break;
	case 'q':
		*tile = 220;
		break;
	case 'e':
		*tile = 221;
		break;
	case 's':
		*tile = 222;
		break;
	case 't':
		*tile = 223;
		break;
	default:
		ASSERT(0 /* invalid title letter */, __ret);
	}

	rv = 0;
__ret:
	return rv;
}


int title_new(struct entity **entity, struct mapObject *data) {
	struct entity *ret = 0;
	struct title tmp = {0};
	int rv = 1;
	int tile;

	ASSERT_OK(title_getTile(&tile, &tmp.isRainbow, data), __ret);
	tmp.ox = data->x;
	tmp.oy = data->y;

	if (tmp.isRainbow) {
		int i;

		for (i = 0; i < TITLE_NUM_TRAIL; i++) {
			tmp.trailTimerMs[i] = -TRAIL_DELAY_MS * (i + 1);
		}
	}

	ASSERT_OK(
		entity_init(
			&tmp.base
			, tmp.ox
			, tmp.oy
			, data->width
			, data->height
			, gfx32x32
			, TYP_TITLE
		)
		, __ret
	);

	ASSERT_OK(gfmSprite_setFrame(tmp.base.sprite, tile), __ret);

	tmp.base.fn.preUpdate = title_preUpdate;
	tmp.base.fn.draw = title_draw;
	tmp.base.fn.free = title_free;

	ASSERT((ret = malloc(sizeof(tmp))) != 0, __ret);
	memcpy(ret, &tmp, sizeof(tmp));
	memset(&tmp, 0x0, sizeof(tmp));

	*entity = ret;

	rv = 0;
__ret:
	if (rv != 0) {
		if (ret) {
			free(ret);
		}
	}

	entity_free(&tmp.base);
	return rv;
}
