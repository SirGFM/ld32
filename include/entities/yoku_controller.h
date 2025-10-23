#ifndef YOKU_CONTROLLER_H
#define YOKU_CONTROLLER_H


#include <core/map.h>
#include <entity.h>


struct yokuController {
	/** The base entity. */
	struct entity base;
	/** ID of the contoller, so it may configure its blocks' timing. */
	int id;
	/** How long the controller's entire cycle takes, in milliseconds. */
	int cycleMs;
};


/**
 * yokuController_new allocates and initializes a new yoku controller.
 *
 * @param [out] entity: The new yoku controller.
 * @param [in] data: The parsed data for the yoku block.
 * @return 0: Success; Anything else: failure.
 */
int yokuController_new(struct entity **entity, struct mapObject *data);


#endif /* YOKU_CONTROLLER_H */
