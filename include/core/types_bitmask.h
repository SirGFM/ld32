#ifndef TYPES_BITMASK_H
#define TYPES_BITMASK_H


/** Mask that return the proper 16 bit type */
#define T_MASK 0x0000ffff
/** Number of bits per type */
#define T_BITS 16
/**
 * How many bits there are for any given "base type". Different types that share
 * the same base one will be rendered within the quadtree with the same color.
 */
#define T_BASE_NBITS 5
/** Mask that returns the type's base type */
#define T_BASE_MASK  0x0000001f


/**
 * GET_TYPE retrieves an object's type,
 * masking out all non-type bits
 * (that can be used for other things.
 *
 * type - The object's type.
 */
#define GET_TYPE(type) ((type) & T_MASK)


/**
 * GET_BASE_TYPE retrieves an object's base type.
 * These types can be used to group similar objects together,
 * to simplify collision handling.
 *
 * type - The object's type.
 */
#define GET_BASE_TYPE(type) ((type) & T_BASE_MASK)


/**
 * GET_EXTRA_TYPE_DATA retrieves any extra data encoded into the object's type.
 *
 * type - The object's type.
 */
#define GET_EXTRA_TYPE_DATA(type) ((type) >> T_BITS)


/**
 * NEW_TYPE generates a new type from a base type and a counter.
 *
 * base - An enum baseType.
 * count - A non-zero counter to uniquely identify this type within the base.
 */
#define NEW_TYPE(base, count) ((count << T_BASE_NBITS) | base)


/**
 * MERGE_TYPES merges two types into a single int,
 * encoding one in the T_BITS higher bits and the other in the lower ones.
 *
 * This is useful for easily comparing two interacting types.
 *
 * type1 - One of the object's type.
 * type2 - The other object's type.
 */
#define MERGE_TYPES(type1, type2) (GET_TYPE(type1) | (GET_TYPE(type2) << T_BITS))


#endif /* TYPES_BITMASK_H */
