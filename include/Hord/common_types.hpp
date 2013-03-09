/**
@file common_types.hpp
@brief Common types.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_COMMON_TYPES_HPP_
#define HORD_COMMON_TYPES_HPP_

#include "./config.hpp"

namespace Hord {

/**
	@addtogroup object
	@{
*/

/** Generic object ID. */
typedef uint32_t ObjectID;

enum : ObjectID {
	/** Null object. */
	OBJECT_NULL=0
};

/** @} */ // end of doc-group object

/**
	@addtogroup metadata
	@{
*/

/** MetaField type. */
typedef uint8_t MetaFieldType;

/** @} */ // end of doc-group metadata

/**
	@addtogroup driver
	@{
*/
/**
	@addtogroup hive
	@{
*/

/**
	Hive ID.

	@note This type is only to telegraph intent in functions;
	it is entirely equivalent to ObjectID.
*/
typedef ObjectID HiveID;

/** @} */ // end of doc-group hive
/** @} */ // end of doc-group driver

/**
	@addtogroup node
	@{
*/

/**
	Node ID.

	@note This type is only to telegraph intent in functions;
	it is entirely equivalent to ObjectID.
*/
typedef ObjectID NodeID;

/**
	@addtogroup rule
	@{
*/

/**
	Rule ID.

	@note This type is only to telegraph intent in functions;
	it is entirely equivalent to ObjectID.
*/
typedef ObjectID RuleID;
/** Rule type. */
typedef uint32_t RuleType;

/** @} */ // end of doc-group rule
/** @} */ // end of doc-group node

} // namespace Hord

#endif // HORD_COMMON_TYPES_HPP_
