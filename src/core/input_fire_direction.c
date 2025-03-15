#include <core/input.h>
#include <error.h>
#include <util.h>

#include <GFraMe/gfmInput.h>


/**
 * _input_getNormalizedGamepad retrieves the normalized value for the specified analog input.
 *
 * The value is properly clamped within the specified deadzone/limit.
 *
 * @param [out] x: The normalized horizontal value.
 * @param [out] y: The normalized vertical value.
 * @param [in] analog: The analog input that should be queried.
 * @return 0: Success; Anything else: failure.
 */
static int _input_getNormalizedGamepad(double *x, double *y, gfmInputIface analog) {
	float dx, dy;
	int rv = 1;

	/* TODO: Handle no gamepad being present. */

	ASSERT(
		GFMRV_OK == gfmInput_getGamepadAnalog(
			&dx,
			&dy,
			inputCtx,
			0, /* TODO: Get the port from somewhere. */
			analog
		)
		, __ret
	);

	/* TODO: Use a configurable analog threshold. */
	flinear_map(&dx, 0.15f, 0.9f);
	flinear_map(&dy, 0.15f, 0.9f);

	*x = (double)dx;
	*y = (double)dy;

	normalize(x, y);

	rv = 0;
__ret:
	return rv;
}


int input_getFireDirection(double *x, double *y, int playerX, int playerY, int playerRight) {
	double mult;
	int rv = 1;

	/* Shoot backward when jumping,
	 * forward when using the fire button. */
	if (input_isPressed(INPUT_JUMP) || input_isPressed(INPUT_JUMP_MOUSE)) {
		mult = -1.0;
	}
	else if (input_isPressed(INPUT_FIRE) || input_isPressed(INPUT_FIRE_MOUSE)) {
		mult = 1.0;
	}
	else {
		ASSERT(0 /* no action pressed */, __ret);
	}

	/* 1. Check if the input came from a mouse event. */
	if (input_isPressed(INPUT_JUMP_MOUSE) || input_isPressed(INPUT_FIRE_MOUSE)) {
		int mouseX, mouseY;

		ASSERT(
			GFMRV_OK == gfmInput_getPointerPosition(
				&mouseX
				, &mouseY
				, inputCtx
			)
			, __ret
		);

		*x = (double)(mouseX - playerX);
		*y = (double)(mouseY - playerY);
		normalize(x, y);
	}
	else {
		*x = 0.0;
		*y = 0.0;

		/* 2. Try to use the right analog if from a fire action. */
		if (input_isPressed(INPUT_FIRE)) {
			ASSERT_OK(
				_input_getNormalizedGamepad(x, y, gfmController_rightAnalog)
				, __ret
			);
		}

		/* 3. Try to use the left analog. */
		if (*x == 0.0 && *y == 0.0) {
			ASSERT_OK(
				_input_getNormalizedGamepad(x, y, gfmController_leftAnalog)
				, __ret
			);
		}

		/* 4. Try to use the directionals. */
		if (*x == 0.0 && *y == 0.0) {
			if (input_isPressed(INPUT_LEFT)) {
				*x = -1.0;
			}
			else if (input_isPressed(INPUT_RIGHT)) {
				*x = 1.0;
			}

			if (input_isPressed(INPUT_UP)) {
				*y = -1.0;
			}
			else if (input_isPressed(INPUT_DOWN)) {
				*y = 1.0;
			}

			if (*x != 0.0 && *y != 0.0) {
				normalize(x, y);
			}
		}

		if (*x == 0.0 && *y == 0.0) {
			/* 5. Default to shooting forward for fire action.
			 * 6. Default to shooting downward for jump action. */
			if (input_isPressed(INPUT_FIRE)) {
				if (playerRight) {
					*x = 1.0;
				}
				else {
					*x = -1.0;
				}
			}
			else {
				/* Since jumping shoots backward,
				 * assign the inverted value. */
				*y = -1.0;
			}
		}
	}

	/* Adjust the firing direction based on the pressed action. */
	*x *= mult;
	*y *= mult;

	rv = 0;
__ret:
	return rv;
}
