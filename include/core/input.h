#ifndef INPUT_H
#define INPUT_H


#include <GFraMe/gfmInput.h>

#include <stdint.h>


/** The game's input context. */
extern gfmInput *inputCtx;


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
		X_INPUT(gfmKey_backspace), \
		X_INPUT(gfmKey_lctrl), \
		X_INPUT(gfmController_b), \
		X_INPUT(gfmController_x), \
		X_INPUT(gfmController_r2), \
	) \
	X( \
		INPUT_SELECTOR, "SELECTOR",/*auto*/, \
		X_INPUT(gfmKey_lshift), \
		X_INPUT(gfmKey_rshift), \
	) \
	X( \
		INPUT_JUMP_MOUSE, "JUMP MOUSE",/*auto*/, \
		X_INPUT(gfmPointer_leftButton), \
	) \
	X( \
		INPUT_FIRE_MOUSE, "FIRE MOUSE",/*auto*/, \
		X_INPUT(gfmPointer_rightButton), \
	) \
	X( \
		INPUT_SELECTOR_MOUSE, "SELECTOR MOUSE",/*auto*/, \
		X_INPUT(gfmPointer_middleButton), \
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


/**
 * input_getFireDirection retrieves the direction at which the rainbow is being shot,
 * in the [-1.0, 1.0] range.
 *
 * The action that triggered this (either a jump or the fire button)
 * determines whether this is shot forward or backward.
 * The movement direction should always point opposite to the returned value.
 *
 * If neither action is active, this returns an error.
 *
 * Lastly, depending on which inputs are enabled,
 * the direction is calculated based on.
 * Note that if one of the conditions isn't met,
 * the next one is attempted.
 *
 *     - Mouse position (if triggered from a mouse event);
 *     - Right analog (if triggered from a fire action);
 *     - Left analog;
 *     - Directional keys (if both analogs are within their deadzone);
 *     - Straight forward (if triggered from a jump action);
 *     - Straight down (if triggered from a jump action).
 *
 * @param [out] x: The horizontal direction.
 * @param [out] y: The vertical direction.
 * @param [in] playerX: The player's horizontal position, in screen space.
 * @param [in] playerY: The player's vertical position, in screen space.
 * @param [in] playerRight: Whether the player is facing right.
 * @return 0: Success; Anything else: failure.
 */
int input_getFireDirection(double *x, double *y, int playerX, int playerY, int playerRight);


#endif /* INPUT_H */
