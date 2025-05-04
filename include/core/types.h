#ifndef TYPES_H
#define TYPES_H


#include <core/types_bitmask.h>

#include <GFraMe/gfmTypes.h>


/**
 * List of types.
 *
 * Types marked as TM shall be used exclusively in tilemaps,
 * and thus appear in tilemapTypes and other arrays.
 *
 * Similarly, TM_SIDED are also tilemap types,
 * but single-sided colliders are generated for these tiles..
 *
 * Other types should be defined with a simple X,
 * which just generates the type information
 * but without adding the type to the tilemap list.
 *
 * arg1 - The type enumeration.
 * arg2 - The name of the type.
 * arg3 - (optional) Assigment expression to initialize the enum for the type.
 */
#define TYPES \
	TM_SIDED(TYP_FLOOR, "floor", = NEW_TYPE(BTYP_FLOOR, 1)) \
	TM(TYP_SPIKES, "spikes", = NEW_TYPE(BTYP_HAZARD, 1)) \
	X(TYP_PLAYER, "player", = NEW_TYPE(BTYP_PLAYER, 1)) \
	X(TYP_TITLE, "title", = NEW_TYPE(BTYP_MENU, 1)) \
	X(TYP_OPTION, "option",) \
	X(TYP_LOADER, "loader", = NEW_TYPE(BTYP_TRIGGER, 1))


/**
 * Base types that groups differents objects together.
 * Also defines the color used to debug the object.
 */
enum baseType {
	  BTYP_HAZARD  = gfmType_reserved_2  /* ( 5) pink */
	, BTYP_PLAYER  = gfmType_reserved_3  /* ( 6) light blue */
	, BTYP_FLOOR   = gfmType_reserved_5  /* ( 8) purple */
	, BTYP_TRIGGER = gfmType_reserved_6  /* ( 9) yellow */
	, BTYP_MENU    = gfmType_reserved_28 /* no color */
};


/**
 * Types used for collision.
 */
enum type {
	TYP_NONE,
#define TM_SIDED(typ, name, expr) \
	typ expr,
#define TM(typ, name, expr) \
	typ expr,
#define X(typ, name, expr) \
	typ expr,
	TYPES
#undef X
#undef TM
#undef TM_SIDED
};


/** Number of tilemap types. */
extern int numTilemapTypes;

/** List of types in the tilemap. */
extern int tilemapTypes[];

/** List of names for each type in the tilemap. */
extern char *tilemapTypeNames[];

/** List of types that have different a collider for each direction. */
extern int sidedCollisions[];

/** Number of types with differente colliders for each direction. */
extern int sidedCollisionsLen;


/**
 * type_get retrieves the type for the given name.
 *
 * @param [out] type: The type's value.
 * @param [in] name: The name of the type.
 * @param [in] len: The name's length.
 * @return 0: Success; Anything else: failure.
 */
int type_get(enum type *type, char *name, int len);


#endif /* TYPES_H */
