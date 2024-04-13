#include <entities/new_by_type.h>
#include <entities/player.h>

int entity_newByType(struct entity **entity, struct mapObject *data) {
	switch (data->type){
	case TYP_PLAYER: return player_new(entity, data->x, data->y);
	// XXX: Add other entities.
	default: return 1;
	}
}
