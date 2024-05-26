#ifndef OPTION_H
#define OPTION_H


#include <core/map.h>
#include <entity.h>
#include <menus/generic_menu.h>

#include <GFraMe/gfmText.h>


struct option {
	/** The base entity. */
	struct entity base;
	/** The underlying text. */
	gfmText *text;
	/** The option activated by pressing 'up'. */
	struct option *prev;
	/** The option activated by pressing 'down'. */
	struct option *next;
	/** Whether this options is currently active. */
	int isActive;
	/** Whether this options was active on the previous frame. */
	int wasActive;
	/** The type of menu in which this options was loaded. */
	enum menu type;
	/** The index of the option in the scene. */
	int idx;
};


/**
 * option_new allocates and initializes a new menu option entry.
 *
 * @param [out] entity: The new option.
 * @param [in] data: The option's configuration.
 * @return 0: Success; Anything else: failure.
 */
int option_new(struct entity **entity, struct mapObject *data);


#endif /* OPTION_H */
