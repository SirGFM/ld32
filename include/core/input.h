#ifndef INPUT_H
#define INPUT_H


#include <GFraMe/gfmInput.h>

#include <stdint.h>


/** Define a single button */
struct button {
	/** Button handle, internal to the framework */
	int handle;
	/** Number of times the button was consecutivelly pressed */
	int numPressed;
	/** Current state of the button */
	gfmInputState state;
	/** Ignore; Used to expand the struct to 32 bytes */
	int padding;
};


/** The list of available inputs in the game. */
#define INPUT_LIST \
	X( \
		INPUT_UP, "UP", = 0, \
		X_INPUT(gfmKey_up), \
		X_INPUT(gfmKey_w), \
		X_INPUT(gfmController_laxis_up), \
	) \
	X( \
		INPUT_DOWN, "DOWN",/*auto*/, \
		X_INPUT(gfmKey_down), \
		X_INPUT(gfmKey_s), \
		X_INPUT(gfmController_laxis_down), \
	) \
	X( \
		INPUT_LEFT, "LEFT",/*auto*/, \
		X_INPUT(gfmKey_left), \
		X_INPUT(gfmKey_a), \
		X_INPUT(gfmController_laxis_left), \
	) \
	X( \
		INPUT_RIGHT, "RIGHT",/*auto*/, \
		X_INPUT(gfmKey_right), \
		X_INPUT(gfmKey_d), \
		X_INPUT(gfmController_laxis_right), \
	) \
	X( \
		INPUT_JUMP, "JUMP",/*auto*/, \
		X_INPUT(gfmKey_space), \
		X_INPUT(gfmKey_return), \
		X_INPUT(gfmController_a), \
		X_INPUT(gfmController_r1), \
	) \
	X( \
		INPUT_FIRE, "FIRE",/*auto*/, \
		X_INPUT(gfmController_b), \
		X_INPUT(gfmController_x), \
		X_INPUT(gfmController_r2), \
	) \
	X( \
		INPUT_FIRE_UP, "FIRE UP",/*auto*/, \
		X_INPUT(gfmController_raxis_up), \
	) \
	X( \
		INPUT_FIRE_DOWN, "FIRE DOWN",/*auto*/, \
		X_INPUT(gfmController_raxis_down), \
	) \
	X( \
		INPUT_FIRE_LEFT, "FIRE LEFT",/*auto*/, \
		X_INPUT(gfmController_raxis_left), \
	) \
	X( \
		INPUT_FIRE_RIGHT, "FIRE RIGHT",/*auto*/, \
		X_INPUT(gfmController_raxis_right), \
	) \
	DEBUG_INPUT_LIST \
	X(INPUT_MAX, "MAX",/*auto*/) \
	/* INPUT_ACCEPT is basically an alias for INPUT_JUMP. */ \
	X(INPUT_ACCEPT, "", = INPUT_JUMP)


/** The list of available debug inputs in the game. */
#if defined(DEBUG)
#  define DEBUG_INPUT_LIST \
	X(DEBUG_INPUT, "DBG",/*auto*/) \
	X( \
		DEBUG_INPUT_PAUSE, "DBG PAUSE", = DEBUG_INPUT, \
		X_INPUT(gfmKey_f9), \
	) \
	X( \
		DEBUG_INPUT_STEP, "DBG STEP",/*auto*/, \
		X_INPUT(gfmKey_f10), \
	) \
	X( \
		DEBUG_INPUT_QT, "DBG QT",/*auto*/, \
		X_INPUT(gfmKey_f12), \
	)
#else /* if !defined(DEBUG) */
#  define DEBUG_INPUT_LIST
#endif /* defined(DEBUG) */


/** List of available inputs in the game. */
enum input {
#define X(typ, name, expr, ...) \
	typ expr,
	INPUT_LIST
#undef X
};


/** Structure used to map a given input. */
struct buttonMapping {
	/** The button's action (an enum input). */
	uint8_t action;
	/** The key used to activated this button (a gfmInputIface). */
	uint8_t key;
	/** The gamepad port that triggers this key. */
	uint8_t port;
};


/**
 * input_isPressed checks if the input is currently pressed.
 *
 * @param [in] action: The input.
 * @return 0: If released; 1: If pressed.
 */
int input_isPressed(enum input action);


/**
 * input_isJustPressed checks if the input was pressed this frame.
 *
 * Note that 0 means that either the input was pressed in a previous frame,
 * or that it's released.
 *
 * @param [in] action: The input.
 * @return 1: If pressed this frame; 0: Otherwise.
 */
int input_isJustPressed(enum input action);


/**
 * input_isJustReleased checks if the input was released this frame.
 *
 * Note that 0 means that either the input was released in a previous frame,
 * or that it's pressed.
 *
 * @param [in] action: The input.
 * @return 1: If released this frame; 0: Otherwise, 
 */
int input_isJustReleased(enum input action);


/**
 * input_init initializes the button subsystem.
 *
 * It tries to load the previously saved button configuration,
 * but defaults to the hard-coded one if not found.
 *
 * @return 0: Success; Anything else: failure.
 */
int input_init();


/**
 * input_updateDebug retrieves the state of every button,
 * forcefully updating the state of the keys, if the game is paused.
 *
 * @return 0: Success; Anything else: failure.
 */
int input_updateDebug();


/**
 * input_update retrieves the state of every button.
 *
 * @return 0: Success; Anything else: failure.
 */
int input_update();


/**
 * input_rebindAll rebinds the actions to the supplied mappings.
 *
 * @param [in] mappings: The desired mapping.
 * @param [in] count: The number of entries in mappings.
 * @return 0: Success; Anything else: failure.
 */
int input_rebindAll(struct buttonMapping *mappings, int count);


#endif /* INPUT_H */
