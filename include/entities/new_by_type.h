#ifndef NEW_BY_TYPE_H
#define NEW_BY_TYPE_H

#include <core/map.h>
#include <entity.h>

/**
 * entity_newByType instantiates a new entity based on the provided data.
 *
 * @param [out] entity: The instantiated custom entity.
 * @param [in] data: This instance's specific data (type, position, etc).
 * @return 0: Success; Anything else: failure.
 */
int entity_newByType(struct entity **entity, struct mapObject *data);

#endif /* NEW_BY_TYPE_H */
