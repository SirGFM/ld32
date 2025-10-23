#include <entities/menus/option.h>
#include <entities/menus/title.h>
#include <entities/new_by_type.h>
#include <entities/loader.h>
#include <entities/player.h>
#include <entities/yoku_block.h>
#include <entities/yoku_controller.h>

int entity_newByType(struct entity **entity, struct mapObject *data) {
	switch (data->type){
	case TYP_PLAYER: return player_new(entity, data->x, data->y);
	case TYP_TITLE: return title_new(entity, data);
	case TYP_OPTION: return option_new(entity, data);
	case TYP_LOADER: return loader_new(entity, data);
	case TYP_YOKU_BLOCK: return yokuBlock_new(entity, data);
	case TYP_YOKU_CONTROLLER: return yokuController_new(entity, data);
	// XXX: Add other entities.
	default: return 1;
	}
}
