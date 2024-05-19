#include <core/core.h>
#include <core/input.h>
#include <error.h>

#include <GFraMe/gframe.h>


/** List of every virtual button in the game. */
static struct button buttons[INPUT_MAX] = {0};

/**
 * Default inputs,
 * forward-declared from the end of the file for better organization).
 */
struct buttonMapping defaultMapping[];

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

	for (i = 0; i < INPUT_MAX; i++) {
		int *handle = &buttons[i].handle;

		ASSERT(GFMRV_OK == gfm_addVirtualKey(handle, gameCtx), __ret);
	}

	rv = 0;
__ret:
	return rv;
}


int input_update() {
	int rv = 1;

	rv = 0;
__ret:
	return rv;
}


#include <variadic/x_create_input.h>


struct buttonMapping defaultMapping[] = {
	INPUT_LIST
};
