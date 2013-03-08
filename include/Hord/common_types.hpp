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

/** Generic object ID. */
typedef uint32_t ObjectID;

enum : ObjectID {
	/** Null object. */
	OBJECT_NULL
};

/**
	@addtogroup metadata
	@{
*/

/** MetaField type. */
typedef uint8_t MetaFieldType;

/** @} */ // end of doc-group metadata

/**
	@addtogroup node
	@{
*/

/** Node ID. */
typedef ObjectID NodeID;

/**
	@addtogroup rule
	@{
*/

/** Rule ID. */
typedef ObjectID RuleID;
/** Rule type. */
typedef uint32_t RuleType;

/** @} */ // end of doc-group rule
/** @} */ // end of doc-group node

} // namespace Hord

#endif // HORD_COMMON_TYPES_HPP_
