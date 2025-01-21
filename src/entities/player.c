#include <error.h>
#include <config/config.h>
#include <core/assets.h>
#include <core/input.h>
#include <core/types.h>
#include <game_math.h>
#include <scene.h>
#include <entities/player.h>

#include <GFraMe/gfmSprite.h>

#include <stdlib.h>
#include <string.h>


/** The player's width, in pixels. */
#define PLAYER_WIDTH 8

/** The player's height, in pixels. */
#define PLAYER_HEIGHT 12

/** The player's jump height, in 8x8 tiles. */
#define PLAYER_JUMP_HEIGHT 2.75

/** Time to reach the appeax of the jump, in frames. */
#define PLAYER_JUMP_TIME 25

/** Time to reach the initial jumping height from the appeax of the jump, in frames. */
#define PLAYER_FALL_TIME 20

#define PLAYER_VX SPEED(360, 30)

/** The initial vertical speed for jumps. */
#define PLAYER_JUMP_VY JUMP_SPEED(PLAYER_JUMP_TIME, PLAYER_JUMP_HEIGHT)

/** The gravity acting on the player on the way up. */
#define PLAYER_JUMP_ACC JUMP_ACCELERATION(PLAYER_JUMP_TIME, PLAYER_JUMP_HEIGHT)

/** The gravity acting on the player on the way down. */
#define PLAYER_FALL_ACC JUMP_ACCELERATION(PLAYER_FALL_TIME, PLAYER_JUMP_HEIGHT)


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
	int rv = 1;
	gfmCollision dir;

	ASSERT(GFMRV_OK == gfmSprite_getCollision(&dir, player->base.sprite), __ret);

	if (dir & gfmCollision_down) {
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
		if (input_isJustPressed(INPUT_JUMP)) {
			ASSERT(
				GFMRV_OK == gfmSprite_setVerticalVelocity(
					player->base.sprite
					, PLAYER_JUMP_VY
				)
				, __ret
			);
		}
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
