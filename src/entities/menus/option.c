#include <error.h>
#include <core/assets.h>
#include <config/config.h>
#include <core/core.h>
#include <core/input.h>
#include <core/map.h>
#include <core/store.h>
#include <core/types.h>
#include <scene.h>
#include <entities/menus/option.h>
#include <menus/generic_menu.h>

#include <GFraMe/gfmText.h>

#include <stdlib.h>
#include <string.h>


/** The dimension of a given tile. */
#define TILE_DIMENSION 8
/** The maximum number of characters in a given line. */
#define MAX_WIDTH (VWIDTH / TILE_DIMENSION)
/** The number of characters revealed per second. */
#define CHARS_PER_SEC 20


/** How long until each character is revealed (in milliseconds). */
#define CHAR_DELAY_MS (1000 / CHARS_PER_SEC)


/**
 * option_onLoad handles user inputs.
 *
 * @param [in] entity: The option's embedded entity.
 * @param [in] scene: The scene that called this function.
 * @return 0: Success; Anything else: failure.
 */
static int option_onLoad(struct entity *entity, struct scene *scene) {
	struct option *first = 0;
	struct option *last = 0;
	struct option *self = (struct option*)entity;
	int i;

	for (i = 0; i < scene->numEntities; i++) {
		struct option *other;
		struct entity *cur = scene->entities[i];

		if (cur->type != TYP_OPTION) {
			continue;
		}

		other = (struct option*)cur;

		/* Keep track of both the first and the last options. */
		if (other->idx == 0) {
			first = other;
		}
		if (last == 0 || other->idx > last->idx) {
			last = other;
		}

		/* Update the link between the adjacent options. */
		if (other->idx == self->idx - 1) {
			self->prev = other;
		}
		else if (other->idx == self->idx + 1) {
			self->next = other;
		}
	}

	/* Update the loop at the edges of the list. */
	first->prev = last;
	last->next = first;

	return 0;
}


/**
 * option_preUpdate handles user inputs.
 *
 * @param [in] entity: The option's embedded entity.
 * @param [in] scene: The scene that called this function.
 * @return 0: Success; Anything else: failure.
 */
static int option_preUpdate(struct entity *entity, struct scene *scene) {
	struct option *self = (struct option*)entity;
	struct option *new = 0;
	int rv = 1;

	if (!self->isActive || !self->wasActive) {
		/* Option isn't or just became active, do nothing!. */
	}
	else if (input_isJustPressed(INPUT_UP)) {
		new = self->prev;
	}
	else if (input_isJustPressed(INPUT_DOWN)) {
		new = self->next;
	}
	else if (input_isJustPressed(INPUT_ACCEPT)) {
		ASSERT_OK(menu_accept(self->type, self->idx), __ret);
	}

	if (new != 0) {
		self->isActive = 0;
		new->isActive = 1;

		ASSERT_OK(
			menu_setText(
				self->text
				, self->type
				, self->idx
				, self->isActive
			)
			, __ret
		);
		ASSERT_OK(
			menu_setText(
				new->text
				, new->type
				, new->idx
				, new->isActive
			)
			, __ret
		);
	}

	ASSERT(GFMRV_OK == gfmText_update(self->text, gameCtx), __ret);

	self->wasActive = self->isActive;

	rv = 0;
__ret:
	return rv;
}


/**
 * option_draw draws the current options.
 *
 * @param [in] entity: The option's embedded entity.
 * @param [in] scene: The scene that called this function.
 * @return 0: Success; Anything else: failure.
 */
static int option_draw(struct entity *entity, struct scene *scene) {
	struct option *option = (struct option*)entity;
	int rv = 1;

	ASSERT(GFMRV_OK == gfmText_draw(option->text, gameCtx), __ret);

	rv = 0;
__ret:
	return rv;
}


/**
 * option_static_free releases every resource allocated into a option,
 * except by the option's memory itself.
 *
 * Therefore, this is self to be called on a stack-allocated option.
 *
 * @param [in] option: The option's embedded entity.
 * @return 0: Success; Anything else: failure.
 */
static int option_static_free(struct option *option) {
	entity_free(&option->base);
	if (option->text) {
		gfmText_free(&option->text);
	}
	return 0;
}


/**
 * option_free releases every resource allocated by option_new.
 *
 * @param [in] entity: The option's embedded entity.
 * @param [in] scene: The scene that called this function.
 * @return 0: Success; Anything else: failure.
 */
static int option_free(struct entity *entity, struct scene *scene) {
	struct option *option = (struct option*)entity;

	option_static_free(option);

	free(option);
	return 0;
}


/**
 * _option_center centers the text horizontally on screen.
 *
 * @param [in] option: 
 * @param [in] y: The vertical position;
 * @return 0: Success; Anything else: failure.
 */
static int _option_center(struct option *option, int y) {
	char *text;
	int len;
	int x;
	int rv = 1;

	ASSERT_OK(menu_getText(&text, &len, option->type, option->idx), __ret);
	x = TILE_DIMENSION * ((MAX_WIDTH - len) / 2);

	ASSERT(GFMRV_OK == gfmText_setPosition(option->text, x, y), __ret);

	rv = 0;
__ret:
	return rv;
}


int option_new(struct entity **entity, struct mapObject *data) {
	char *tag;
	struct entity *ret = 0;
	struct option tmp = {0};
	int rv = 1;
	int len;

	/* Load the option's type and index. */
	ASSERT(data->numAttribute == 1, __ret);
	ASSERT_OK(menu_getType(&tmp.type, data->attributes[0].type), __ret);
	ASSERT_OK(store_getStr(&tag, &len, data->attributes[0].value), __ret);
	ASSERT_OK(menu_getIdx(&tmp.idx, tmp.type, tag, len), __ret);
	/* Start with the first entry active. */
	tmp.isActive = (tmp.idx == 0);

	/* Load the actual text. */
	ASSERT(GFMRV_OK == gfmText_getNew(&tmp.text), __ret);
	ASSERT(
		GFMRV_OK == gfmText_init(
			tmp.text
			, data->x
			, data->y
			, MAX_WIDTH
			, 1 /*number of lines*/
			, CHAR_DELAY_MS
			, 0/*bind to screen*/
			, gfx8x8
			, 0/*first tile*/
		)
		, __ret
	);
	ASSERT_OK(menu_setText(tmp.text, tmp.type, tmp.idx, tmp.isActive), __ret);

	/* Center the text. */
	ASSERT_OK(_option_center(&tmp, data->y), __ret);

	/* Load a dummy sprite (required by the entity. */
	ASSERT_OK(
		entity_init(
			&tmp.base
			, data->x
			, data->y
			, TILE_DIMENSION
			, TILE_DIMENSION
			, gfx8x8
			, TYP_OPTION
		)
		, __ret
	);

	/* Configure the vtable. */
	tmp.base.fn.onLoad = option_onLoad;
	tmp.base.fn.preUpdate = option_preUpdate;
	tmp.base.fn.draw = option_draw;
	tmp.base.fn.free = option_free;

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

	option_static_free(&tmp);
	return rv;
}
