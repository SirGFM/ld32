#ifndef CREATE_INPUT_H
#define CREATE_INPUT_H


#include <variadic/num_args.h>


/**
 * X creates all the default biding for a single action.
 *
 * Actions may be bound to an input key
 * (either a keyboard key or a gamepad button)
 * using the X_INPUT macro.
 *
 * For example, a given action may look like:
 *
 * 	X( \
 * 		INPUT_UP, "UP", = 0, \
 * 		X_INPUT(gfmKey_left), \
 * 		X_INPUT(gfmKey_a), \
 * 		X_INPUT(gfmController_laxis_left), \
 * 	)
 *
 * Even if an action's value is automatically initialized,
 * its empty initialization expression must be followed by a comma.
 * E.g.:
 *
 * 	X( \
 * 		INPUT_FIRE_UP, "FIRE UP", , \ // Note the ", ," at the end of the line.
 * 		X_INPUT(gfmController_raxis_up), \
 * 	) \
 *
 * Note that the list of inputs MUST end in a comma.
 * Otherwise, an empty list would be incorrectly detected as not empty.
 * E.g.:
 *
 * 	// Example of an alias to INPUT_JUMP, and thus has no bindings.
 * 	X( \
 * 		(INPUT_ACCEPT, "", = INPUT_JUMP
 * 	)
 *
 * @param action - The enumeration representing the action.
 * @param name - The printable name of the action (ignored).
 * @param expr - The initialization expression for the action (ignored).
 * @param __VA_ARGS__ - Any number of X_INPUT bindings.
 */
#define X(action, name, expr, ...) \
	X_CREATE_VAR(PP_NARG(__VA_ARGS__), action, __VA_ARGS__)


/**
 * X_CREATE_VAR creates a single binding for an input.
 *
 * @param _action - The enumeration representing the action.
 * @param __VA_ARGS__ - A X_INPUT binding.
 */
#define X_CREATE_1(_action, ...) \
	{ \
		.action = (uint8_t)_action, \
		__VA_ARGS__ \
	},


/**
 * X_INPUT creates a single binding.
 *
 * @param _key: The bound key.
 */
#define X_INPUT(_key) \
	.key = (uint8_t)_key


#define X_CREATE_VAR(num, ...) X_CREATE_N(num, __VA_ARGS__)


#define X_CREATE_N(num, ...) X_CREATE_ ## num(__VA_ARGS__)


#define X_CREATE_10(action, args, ...) \
	X_CREATE_1(action, args) \
	X_CREATE_9(action, __VA_ARGS__)


#define X_CREATE_9(action, args, ...) \
	X_CREATE_1(action, args) \
	X_CREATE_8(action, __VA_ARGS__)


#define X_CREATE_8(action, args, ...) \
	X_CREATE_1(action, args) \
	X_CREATE_7(action, __VA_ARGS__)


#define X_CREATE_7(action, args, ...) \
	X_CREATE_1(action, args) \
	X_CREATE_6(action, __VA_ARGS__)


#define X_CREATE_6(action, args, ...) \
	X_CREATE_1(action, args) \
	X_CREATE_5(action, __VA_ARGS__)


#define X_CREATE_5(action, args, ...) \
	X_CREATE_1(action, args) \
	X_CREATE_4(action, __VA_ARGS__)


#define X_CREATE_4(action, args, ...) \
	X_CREATE_1(action, args) \
	X_CREATE_3(action, __VA_ARGS__)


#define X_CREATE_3(action, args, ...) \
	X_CREATE_1(action, args) \
	X_CREATE_2(action, __VA_ARGS__)


#define X_CREATE_2(action, args, ...) \
	X_CREATE_1(action, __VA_ARGS__) \
	X_CREATE_1(action, args)


/* Ignore actions without any binding. */
#define X_CREATE_0(action, args) /* Do nothing. */


#endif /* CREATE_INPUT_H */
