#ifndef GAME_MATH_H
#define GAME_MATH_H


/**
 * SPEED calculates the linear speed, as a double.
 *
 * This uses an arbitrary distance and time to help calculate the speed for entities.
 *
 * @param  time - Time, in 60FPS-frames, to traverse the stipulated distance.
 * @param  distance - Default distance to be traversed, in 8px-tiles.
 */
#define SPEED(time, distance) (TILES_TO_PX(distance) / FRAMES_TO_S(time))


/**
 * JUMP_SPEED calculates the initial jump speed, as a double.
 *
 * @param  timeToAppex - Time, in 60FPS-frames, from the ground to the appex.
 * @param  jumpHeight - Jump height in 8px-tiles.
 */
#define JUMP_SPEED(timeToAppex, jumpHeight) \
	(-2.0 * TILES_TO_PX(jumpHeight) / FRAMES_TO_S(timeToAppex))


/**
 * JUMP_ACCELERATION calculates the gravity acceleration, as a double.
 *
 * @param  timeToAppex - Time, in 60FPS-frames, from the ground to the appex.
 * @param  jumpHeight - Jump height in 8px-tiles.
 */
#define JUMP_ACCELERATION(timeToAppex, jumpHeight) \
	( \
		2.0 * TILES_TO_PX(jumpHeight) / ( \
			FRAMES_TO_S(timeToAppex) * FRAMES_TO_S(timeToAppex) \
		) \
	)


/**
 * TILES_TO_PX converts from tiles to pixels.
 *
 * Each tile is assumed to be 8x8 pixels.
 *
 * @param n - The number of tiles to be converted.
 */
#define TILES_TO_PX(n) (n * 8)


/**
 * FRAMES_TO_S converts from frames to seconds, as a double.
 *
 * To make it framerate independent, this assumes a base FPS of 60.
 *
 * @param n - The number of frames to be converted.
 */
#define FRAMES_TO_S(n) (n / 60.0)


/**
 * FRAMES_TO_MS converts from frames to milliseconds.
 * 
 * To make it framerate independent, this assumes a base FPS of 60.
 *
 * @param n - The number of frames to be converted.
 */
#define FRAMES_TO_MS(n) (n * 16)


#endif /* GAME_MATH_H */
