#include <error.h>
#include <config/config.h>
#include <core/assets.h>
#include <core/core.h>
#include <core/input.h>
#include <core/types.h>
#include <camera.h>
#include <game_math.h>
#include <global.h>
#include <particles/rainbow.h>
#include <scene.h>
#include <util.h>
#include <entities/player.h>

#include <GFraMe/gfmSprite.h>

#include <stdlib.h>
#include <string.h>

#if defined(DEBUG)
#include <GFraMe/gfmDebug.h>

#include <math.h>
#endif /* defined(DEBUG) */


/** The player's width, in pixels. */
#define PLAYER_WIDTH 8

/** The player's height, in pixels. */
#define PLAYER_HEIGHT 12

/** Value used to calculate the player's fly acceleration and max speed.
 * It's calculated as if the player were to jump to that height from the floor. */
#define PLAYER_FLY_HEIGHT 3.0

/** The player's jump height, in 8x8 tiles. */
#define PLAYER_JUMP_HEIGHT 2.75

/** Time to reach the appeax of the jump, in frames. */
#define PLAYER_JUMP_TIME 25

/** Time to reach the initial jumping height from the appeax of the jump, in frames. */
#define PLAYER_FALL_TIME 20

#define PLAYER_VX SPEED(360, 30)

/** How far away from the player are the rainbow particles spawned. */
#define PLAYER_SHOOT_DIST 8

/** The initial vertical speed for jumps. */
#define PLAYER_JUMP_VY JUMP_SPEED(PLAYER_JUMP_TIME, PLAYER_JUMP_HEIGHT)

/** The gravity acting on the player on the way up. */
#define PLAYER_JUMP_ACC JUMP_ACCELERATION(PLAYER_JUMP_TIME, PLAYER_JUMP_HEIGHT)

/** The gravity acting on the player on the way down. */
#define PLAYER_FALL_ACC JUMP_ACCELERATION(PLAYER_FALL_TIME, PLAYER_JUMP_HEIGHT)

/** The (absolute) maximum speed that the player may reach while flying). */
// (PLAYER_JUMP_TIME, 3.5) -> ~3*vx, slightly above fall vy
// (PLAYER_JUMP_TIME, 3.0) -> ~3*vx, slightly bellow fall vy
// (PLAYER_FALL_TIME, 3.5) -> ~4*vx, marginally above fall vy
// (PLAYER_FALL_TIME, 3.0) -> ~3.5*vx, slightly more above fall vy
#define PLAYER_FLY_MAX_SPEED (-JUMP_SPEED(PLAYER_JUMP_TIME, PLAYER_FLY_HEIGHT))

/** The force applied to the player when flying. */
#define PLAYER_FLY_ACC (2 * PLAYER_FALL_ACC)

/** For how long each stone lets the player fly. */
#define PLAYER_STONE_FLY_TIME 1250

/** How much faster thus the fuel recharge if it wasn't fully spent. */
#define PLAYER_FLY_RECHARGE_MODIFIER 3


/** List of animations */
enum animation {
	STAND = 0
	, WALK
	, JUMP
	, FALL
};


/** The player's animation data */
static int animationData[] = {
/*         len|fps|loop|data... */
/* STAND */ 1 , 1 ,  0 , 352,
/*  WALK */ 8 , 10,  1 , 353,354,355,356,357,358,359,360,
/*  JUMP */ 2 , 6 ,  1 , 361,362,
/*  FALL */ 2 , 6 ,  1 , 363,364,
};


static void player_setMaxFlight(struct player *player) {
	player->numStones = countBits((uint32_t)global_getPermanent().stones);
	player->maxFlight = player->numStones * PLAYER_STONE_FLY_TIME;
}


static int player_shoot(struct player *player, struct scene *scene) {
	int rv = 1;

	double shootDirX, shootDirY;
	int camCenterX, centerX, camCenterY, centerY, isLeft, isDown;
	enum rainbowColor bullets, stones, curStoneBit;
	gfmCollision dir;

	ASSERT(GFMRV_OK == gfmSprite_getDirection(&isLeft, player->base.sprite), __ret);

	/* Get the player's position in screen space. */
	ASSERT(GFMRV_OK == gfmSprite_getCenter(&centerX, &centerY, player->base.sprite), __ret);

	camCenterX = centerX;
	camCenterY = centerY;
	ASSERT_OK(camera_worldToScreen(&camCenterX, &camCenterY), __ret);

	/* Get the shooting direction. */
	ASSERT_OK(input_getFireDirection(&shootDirX, &shootDirY, camCenterX, camCenterY, !isLeft), __ret);


	/* Apply the velocity in the opposite direction,
	 * but only if from a jump press or if flying. */
	ASSERT(GFMRV_OK == gfmSprite_getCollision(&dir, player->base.sprite), __ret);
	isDown = (dir & gfmCollision_down);

	if (input_isPressed(INPUT_JUMP) || input_isPressed(INPUT_JUMP_MOUSE) || !isDown) {
		double vx, vy;

		ASSERT(GFMRV_OK == gfmSprite_getVelocity(&vx, &vy, player->base.sprite), __ret);

		vx -= shootDirX * PLAYER_FLY_ACC * ((double)scene->elapsedMs) * 0.001;
		clampAbs(&vx, PLAYER_FLY_MAX_SPEED);
		vy -= shootDirY * PLAYER_FLY_ACC * ((double)scene->elapsedMs) * 0.001;
		clampAbs(&vy, PLAYER_FLY_MAX_SPEED);

		ASSERT(GFMRV_OK == gfmSprite_setVelocity(player->base.sprite, vx, vy), __ret);
	}

	/* Check which bullets should be spawned. */
	stones = global_getPermanent().stones;
	curStoneBit = 1;
	bullets = 0;
	while (curStoneBit <= stones) {
		if (curStoneBit & stones) {
			bullets |= curStoneBit;
		}

		curStoneBit <<= 1;
	}

	/* Spawn the particles. */
	ASSERT_OK(
		rainbow_spawn(
			  bullets
			, shootDirX
			, shootDirY
			, centerX
			, centerY
			, PLAYER_SHOOT_DIST
		)
		, __ret
	);

	rv = 0;
__ret:
	return rv;
}


static void player_recoverFuel(struct player *player, int elapsedMs) {
	if (player->curFlight < player->maxFlight) {
		/* The fuel wasn't fully spent, do the fast recharge. */
		player->curFlight -= elapsedMs * PLAYER_FLY_RECHARGE_MODIFIER;
	}
	else if (player->flightRecharge < PLAYER_STONE_FLY_TIME * player->numStones / PLAYER_FLY_RECHARGE_MODIFIER) {
		/* The fuel was fully spent, wait for the cooldown. */
		player->flightRecharge += elapsedMs;
	}
	else {
		/* The fuel was fully spent, AND the cooldown has elapsed. */
		player->curFlight = 0;
		player->flightRecharge = 0;
	}

	player->curFlight = max(player->curFlight, 0);
}


/**
 * player_preUpdate handles user inputs,
 * preparing the entity's new physics state.
 *
 * @param [in] entity: The player's embedded entity.
 * @param [in] scene: The scene that called this function.
 * @return 0: Success; Anything else: failure.
 */
static int player_preUpdate(struct entity *entity, struct scene *scene) {
	struct player *player = (struct player*)entity;
	double vy, ay;
	int isDown;
	int rv = 1;
	gfmCollision dir;

	ASSERT(GFMRV_OK == gfmSprite_getCollision(&dir, player->base.sprite), __ret);

	isDown = (dir & gfmCollision_down);
	if (isDown) {
		double vx;

		/* Horizontal movement. */
		if (input_isPressed(INPUT_LEFT)) {
			vx = -PLAYER_VX;
		}
		else if (input_isPressed(INPUT_RIGHT)) {
			vx = PLAYER_VX;
		}
		else {
			vx = 0.0;
		}
		ASSERT(
			GFMRV_OK == gfmSprite_setHorizontalVelocity(
				player->base.sprite
				, vx
			)
			, __ret
		);

		/* Jump. */
		if (input_isJustPressed(INPUT_JUMP) || input_isJustPressed(INPUT_JUMP_MOUSE)) {
			ASSERT(
				GFMRV_OK == gfmSprite_setVerticalVelocity(
					player->base.sprite
					, PLAYER_JUMP_VY
				)
				, __ret
			);
		}
	}

	/* Start shooting if the player
	 *   - pressed jump while in the air
	 *   - pressed a fire action regardless of their state
	 */
	if (!player->isShooting) {
		player->isShooting = (
			(
				!isDown
				&& (
					input_isJustPressed(INPUT_JUMP)
					|| input_isJustPressed(INPUT_JUMP_MOUSE)
				)
			)
			|| input_isPressed(INPUT_FIRE)
			|| input_isPressed(INPUT_FIRE_MOUSE)
		);
	} else {
		player->isShooting = (
			input_isPressed(INPUT_JUMP)
			|| input_isPressed(INPUT_JUMP_MOUSE)
			|| input_isPressed(INPUT_FIRE)
			|| input_isPressed(INPUT_FIRE_MOUSE)
		);
	}

	if (player->isShooting && player->curFlight < player->maxFlight) {
		player->curFlight += scene->elapsedMs;
		player->curFlight = min(player->curFlight, player->maxFlight);

		ASSERT_OK(player_shoot(player, scene), __ret);
	}

	if (isDown && player->curFlight > 0) {
		player_recoverFuel(player, scene->elapsedMs);
	}

	/* Set gravity. */
	ASSERT(GFMRV_OK == gfmSprite_getVerticalVelocity(&vy, player->base.sprite), __ret);
	if (vy >= 0) {
		ay = PLAYER_FALL_ACC;
	}
	else {
		ay = PLAYER_JUMP_ACC;
	}
	ASSERT(
		GFMRV_OK == gfmSprite_setVerticalAcceleration(
			player->base.sprite
			, ay
		)
		, __ret
	);

	rv = 0;
__ret:
	return rv;
}


/**
 * player_postUpdate mainly updates the entity's animation.
 *
 * It's called after the physics update
 * and after every object has collided.
 *
 * @param [in] entity: The player's embedded entity.
 * @param [in] scene: The scene that called this function.
 * @return 0: Success; Anything else: failure.
 */
static int player_postUpdate(struct entity *entity, struct scene *scene) {
	double vx, vy;
	struct player *player = (struct player*)entity;
	int rv = 1;
	gfmCollision dir;

	ASSERT(GFMRV_OK == gfmSprite_getVelocity(&vx, &vy, player->base.sprite), __ret);
	ASSERT(GFMRV_OK == gfmSprite_getCollision(&dir, player->base.sprite), __ret);

	if (dir & gfmCollision_down) {
		/* Set walking/standing animation. */
		if (vx != 0.0) {
			ASSERT_OK(entity_playAnimation(entity, WALK, 0), __ret);
		}
		else {
			ASSERT_OK(entity_playAnimation(entity, STAND, 0), __ret);
		}
	}
	else {
		if (vy < 0.0) {
			ASSERT_OK(entity_playAnimation(entity, JUMP, 0), __ret);
		}
		else if (vy > 0.0) {
			ASSERT_OK(entity_playAnimation(entity, FALL, 0), __ret);
		}
	}

	/* Flip horizontally. */
	if (vx > 0.0) {
		ASSERT(GFMRV_OK == gfmSprite_setDirection(player->base.sprite, 0), __ret);
	} else if (vx < 0.0) {
		ASSERT(GFMRV_OK == gfmSprite_setDirection(player->base.sprite, 1), __ret);
	}

	rv = 0;
__ret:
	return rv;
}


/**
 * player_draw draws the player and the rainbow particle behind them.
 *
 * @param [in] entity: The player's embedded entity.
 * @param [in] scene: The scene that called this function.
 * @return 0: Success; Anything else: failure.
 */
static int player_draw(struct entity *entity, struct scene *scene) {
	struct player *player = (struct player*)entity;
	int rv = 1;

	ASSERT_OK(rainbow_draw(scene), __ret);
	ASSERT(GFMRV_OK == gfmSprite_draw(player->base.sprite, gameCtx), __ret);

#if defined(DEBUG)
	do {
		double ax, ay;
		double vx, vy;

		ASSERT(GFMRV_OK == gfmSprite_getAcceleration(&ax, &ay, player->base.sprite), __ret);
		ASSERT(GFMRV_OK == gfmSprite_getVelocity(&vx, &vy, player->base.sprite), __ret);

		gfmDebug_printf(
			gameCtx
			, 0
			, 64
			, "AX: %01d.%01d\n"
			  "VX: %01d.%01d\n"
			  "AY: %01d.%01d\n"
			  "VY: %01d.%01d\n"
			  "MAX FUEL: %06d\n"
			  "CUR FUEL: %06d\n"
			  "RECHARGE: %06d\n"
			, (int)ax
			, abs((int)(100 * (ax - (int)ax)))
			, (int)vx
			, abs((int)(100 * (vx - (int)vx)))
			, (int)ay
			, abs((int)(100 * (ay - (int)ay)))
			, (int)vy
			, abs((int)(100 * (vy - (int)vy)))
			, player->maxFlight
			, player->curFlight
			, player->flightRecharge
		);
	} while (0);
#endif /* defined(DEBUG) */

	rv = 0;
__ret:
	return rv;
}


/**
 * player_static_free releases every resource allocated into a player,
 * except by the player's memory itself.
 *
 * Therefore, this is self to be called on a stack-allocated player.
 *
 * @param [in] entity: The player's embedded entity.
 * @return 0: Success; Anything else: failure.
 */
static int player_static_free(struct player *player) {
	entity_free(&player->base);
	return 0;
}


/**
 * player_free releases every resource allocated by player_new.
 *
 * @param [in] entity: The player's embedded entity.
 * @param [in] scene: The scene that called this function.
 * @return 0: Success; Anything else: failure.
 */
static int player_free(struct entity *entity, struct scene *scene) {
	struct player *player = (struct player*)entity;

	player_static_free(player);
	free(entity);
	return 0;
}


int player_new(struct entity **entity, int x, int y) {
	struct entity *ret = 0;
	struct player tmp = {0};
	int rv = 1;

	/* Adjust the player's flight time. */
	player_setMaxFlight(&tmp);

	ASSERT_OK(
		entity_init(
			&tmp.base
			, x
			, y
			, PLAYER_WIDTH
			, PLAYER_HEIGHT
			, gfx16x16
			, TYP_PLAYER
		)
		, __ret
	);

	/* Load animations */
	ASSERT(
		GFMRV_OK == gfmSprite_addAnimations(
			tmp.base.sprite
			, animationData
			, sizeof(animationData) / sizeof(int)
		)
		, __ret
	);
	ASSERT(GFMRV_OK == gfmSprite_playAnimation(tmp.base.sprite, STAND), __ret);

	tmp.base.fn.preUpdate = player_preUpdate;
	tmp.base.fn.postUpdate = player_postUpdate;
	tmp.base.fn.draw = player_draw;
	tmp.base.fn.free = player_free;

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

	player_static_free(&tmp);
	return rv;
}
