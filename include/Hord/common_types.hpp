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

/**
	Generic object ID.

	@sa HiveID,
		RuleID,
		NodeID
*/
using ObjectID = uint32_t;

/**
	ObjectID constants.
*/
enum : ObjectID {
	/**
		Null object.
	*/
	OBJECT_NULL = 0
};

/**
	@addtogroup hive
	@{
*/

/**
	Hive ID.

	@note This type is only to telegraph intent in functions;
	it is entirely equivalent to ObjectID.

	@sa ObjectID,
		RuleID,
		NodeID
*/
using HiveID = ObjectID;

/** @} */ // end of doc-group hive

/**
	@addtogroup node
	@{
*/

/**
	Node ID.

	@note This type is only to telegraph intent in functions;
	it is entirely equivalent to ObjectID.

	@sa ObjectID,
		HiveID,
		RuleID
*/
using NodeID = ObjectID;

/** @} */ // end of doc-group node

/**
	@addtogroup rule
	@{
*/

/**
	Rule ID.

	@note This type is only to telegraph intent in functions;
	it is entirely equivalent to ObjectID.

	@sa ObjectID,
		HiveID,
		NodeID
*/
using RuleID = ObjectID;

/**
	Rule type.
*/
using RuleType = uint32_t;

/** @} */ // end of doc-group rule
/** @} */ // end of doc-group object

/**
	@addtogroup messaging
	@{
*/

/**
	Message ID.
	@sa MessageBuffer
*/
using MessageID = uint32_t;

/** @} */ // end of doc-group messaging

} // namespace Hord

#endif // HORD_COMMON_TYPES_HPP_
