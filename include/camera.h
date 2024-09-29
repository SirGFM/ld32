#ifndef CAMERA_H
#define CAMERA_H


/**
 * camera_init prepares the camera sub-system.
 *
 * @return 0: Success; Anything else: failure.
 */
int camera_init();


/**
 * camera_update updates any effects/movement applied to the camera.
 *
 * @return 0: Success; Anything else: failure.
 */
int camera_update();


/**
 * camera_setPosition immediately sets the camera's position.
 *
 * @param [in] x: The horizontal position.
 * @param [in] y: The vertical position.
 * @return 0: Success; Anything else: failure.
 */
int camera_setPosition(int x, int y);


/**
 * camera_translate immediately moves the camera current position by some amount.
 *
 * @param [in] dx: The horizontal translation.
 * @param [in] dy: The vertical translation.
 * @return 0: Success; Anything else: failure.
 */
int camera_translate(int dx, int dy);


/**
 * camera_moveToPosition starts a motion of the camera toward the specified point.
 *
 * @param [in] x: The horizontal position.
 * @param [in] y: The vertical position.
 * @param [in] durationMs: How long the motion shall take, in milliseconds.
 * @return 0: Success; Anything else: failure.
 */
int camera_moveToPosition(int x, int y, int durationMs);


/**
 * camera_isMoving checks whether the camera is moving
 * toward a specified point.
 *
 * @return 1: The camera is moving; 0: Otherwise.
 */
int camera_isMoving();


#endif /* CAMERA_H */
