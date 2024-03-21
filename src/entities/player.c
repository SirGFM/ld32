#include <error.h>
#include <core/assets.h>
#include <core/types.h>
#include <scene.h>
#include <entities/player.h>

#include <stdlib.h>
#include <string.h>


#define PLAYER_WIDTH 8
#define PLAYER_HEIGHT 12


/**
 * player_preUpdate handles user inputs,
 * preparing the entity's new physics state.
 *
 * @param [in] entity: The player's embedded entity.
 * @param [in] scene: The scene that called this function.
 * @return 0: Success; Anything else: failure.
 */
static int player_preUpdate(struct entity *entity, struct scene *scene) {
	/* TODO */
	return 0;
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
	/* TODO */
	return 0;
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

	tmp.base.fn.preUpdate = player_preUpdate;
	tmp.base.fn.postUpdate = player_postUpdate;

	/* TODO: Load animations */

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
