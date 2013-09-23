/**
@file Object/Defs.hpp
@brief Object definitions.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_OBJECT_DEFS_HPP_
#define HORD_OBJECT_DEFS_HPP_

#include <Hord/config.hpp>

namespace Hord {
namespace Object {

// Forward declarations
enum class Type : unsigned;
struct type_info;

/**
	@addtogroup object
	@{
*/

/**
	Generic object ID.

	@sa Hive::ID,
		Rule::ID,
		Node::ID
*/
using ID = uint32_t;

/**
	Object::ID constants.
*/
enum : Object::ID {
	/**
		Null object.
	*/
	NULL_ID = static_cast<Object::ID>(0)
};

/**
	%Object type.
*/
enum class Type : unsigned {
	/** @ref hive. */
	Hive,
	/** @ref rule. */
	Rule,
	/** @ref node. */
	Node
};

/**
	Object type info.
*/
struct type_info final {
	/**
		%Object type.

		@sa Object::Type
	*/
	Object::Type const type;
};

/** @} */ // end of doc-group object

} // namespace Object
} // namespace Hord

#endif // HORD_OBJECT_DEFS_HPP_
