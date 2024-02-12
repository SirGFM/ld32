#ifndef ENTITY_H
#define ENTITY_H


#define FORWARD_ONLY
#include <scene.h>
#undef FORWARD_ONLY
#include <core/types.h>

#include <GFraMe/gfmSprite.h>


struct entity;


/**
 * entity_fn defines a custom function implemented by an entity.
 *
 * @param [in] self: The entity itself.
 * @param [in] scene: The scene that called this custom function.
 * @return 0: Success; Anything else: failure.
 */
typedef int(*entity_fn)(struct entity *self, struct scene *scene);


/** The list of functions available to custom entities. */
struct entityVTable {
	/**
	 * An update function called before gfmSprite's update and before the collision.
	 *
	 * Note that the scene's entities are automatically collided.
	 * However, and entity's sub-object must be manually collided.
	 *
	 * The default implementation does nothing!
	 */
	entity_fn preUpdate;
	/**
	 * An update function called after gfmSprite's update and after the collision.
	 *
	 * The default implementation does nothing!
	 */
	entity_fn postUpdate;
	/**
	 * The entity's draw function.
	 *
	 * This may be used this to customize how an object is drawn
	 * (for example, drawing sub-objects).
	 *
	 * The default implementation simply calls gfmSprite's draw.
	 */
	entity_fn draw;
	/**
	 * Releases every resource associated with an entity.
	 *
	 * This must release both the entity's resources
	 * as well as the specialized entity's resources (if any)!
	 *
	 * The default implementation simply frees the entity,
	 * ignoring any specialized entity.
	 */
	entity_fn free;
};


/**
 * A basic wrapper over gfmSprite.
 *
 * This wrapper simplifies defining custom function for different entities:
 * simply overwrite any of the functions in fn.
 *
 * If a custom entity requires more data,
 * it may embed this structure inside itself:
 *
 * 	struct dummyEntity {
 * 		// The base entity.
 * 		struct entity entity;
 * 		// Some custom data required by this entity.
 * 		int foo;
 * 	};
 *
 * This way, the address of the underlying entity shall be the same
 * as the address of the custom type:
 *
 * 	// A dummy pre-update function.
 * 	int dummyEntity_preUpdate(struct entity *entity, struct scene *scene) {
 * 		// "Convert" the entity back to a dummyEntity.
 * 		struct dummyEntity *dummy = (struct dummyEntity*)entity;
 * 		if (dummy->foo > 0) {
 * 			// Do something...
 * 		}
 * 		return 0;
 * 	}
 */
struct entity {
	/** The entity's custom functions. */
	struct entityVTable fn;
	/** The entity's underlying sprite. */
	gfmSprite *sprite;
	/** The entity's underlying type. */
	enum type type;
};


/**
 * Initializes an entity.
 *
 * The sprite's offset is calculate as to center the hitbox horizontally,
 * but keeping the hitbox at the bottom of the image,
 * and the custom functions are configured as the default ones.
 *
 * @param [in] self: The entity itself.
 * @return 0: Success; Anything else: failure.
 */
int entity_init(
	struct entity *entity
	, int x
	, int y
	, int width
	, int height
	, gfmSpriteset *sset
	, enum type type
);


/**
 * entity_free releases every resource associated with an entity.
 *
 * NOTE: This doesn't call entity->fn.free(),
 * so specialized entities may call this function.
 *
 * @param [in] self: The entity itself.
 */
void entity_free(struct entity *entity);


#endif /* ENTITY_H */
