#include <core/core.h>
#include <core/input.h>
#include <error.h>

#include <GFraMe/gframe.h>
#include <GFraMe/gfmInput.h>


/** List of every virtual button in the game. */
static struct button buttons[INPUT_MAX] = {0};

/**
 * Default inputs,
 * forward-declared from the end of the file for better organization).
 */
struct buttonMapping defaultMapping[];
const int numDefaultMapping;

int input_isPressed(enum input action) {
	return (buttons[action].state & gfmInput_pressed) != 0;
}


int input_isJustPressed(enum input action) {
	return (buttons[action].state & gfmInput_justPressed) == gfmInput_justPressed;
}


int input_isJustReleased(enum input action) {
	return (buttons[action].state & gfmInput_justReleased) == gfmInput_justReleased;
}


int input_init() {
	int rv = 1;
	int i;

	ASSERT_OK(input_rebindAll(defaultMapping, numDefaultMapping), __ret);

	rv = 0;
__ret:
	return rv;
}


int input_updateDebug() {
	gfmInput *input;
	int rv = 1;
	int i;

	ASSERT(GFMRV_OK == gfm_getInput(&input, gameCtx), __ret);

	for (i = DEBUG_INPUT; i < INPUT_MAX; i++) {
		struct button *button = buttons + i;

		ASSERT(
			GFMRV_OK == gfmInput_updateVKey(
				input
				, button->handle
			)
			, __ret
		);

		ASSERT(
			GFMRV_OK == gfm_getKeyState(
				&button->state
				, &button->numPressed
				, gameCtx
				, button->handle
			)
			, __ret
		);
	}

	rv = 0;
__ret:
	return rv;
}


int input_update() {
	int rv = 1;
	int i;

	for (i = 0; i < INPUT_MAX; i++) {
		struct button *button = buttons + i;

		ASSERT(
			GFMRV_OK == gfm_getKeyState(
				&button->state
				, &button->numPressed
				, gameCtx
				, button->handle
			)
			, __ret
		);
	}

	rv = 0;
__ret:
	return rv;
}


int input_rebindAll(struct buttonMapping *mappings, int count) {
	int rv = 1;
	int i;

	ASSERT(GFMRV_OK == gfm_resetInput(gameCtx), __ret);

	for (i = 0; i < INPUT_MAX; i++) {
		int *handle = &buttons[i].handle;

		ASSERT(GFMRV_OK == gfm_addVirtualKey(handle, gameCtx), __ret);
	}

	for (i = 0; i < count; i++) {
		struct buttonMapping *mapping = mappings + i;
		int handle = buttons[mapping->action].handle;

		if (mapping->key >= gfmController_left) {
			ASSERT(
				GFMRV_OK == gfm_bindGamepadInput(
					gameCtx
					, handle
					, mapping->key
					, mapping->port
				)
				, __ret
			);
		}
		else {
			ASSERT(
				GFMRV_OK == gfm_bindInput(
					gameCtx
					, handle
					, mapping->key
				)
				, __ret);
		}
	}

	rv = 0;
__ret:
	return rv;
}


#include <variadic/x_create_input.h>


/** The default biding for every action. */
struct buttonMapping defaultMapping[] = {
	INPUT_LIST
};

/** The number of default bidings. */
const int numDefaultMapping = sizeof(defaultMapping) / sizeof(struct buttonMapping);
