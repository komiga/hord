/**
@file Object/Defs.hpp
@brief Object definitions.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_OBJECT_DEFS_HPP_
#define HORD_OBJECT_DEFS_HPP_

#include <Hord/config.hpp>
#include <Hord/IO/PropStateStore.hpp>

#include <duct/cc_unique_ptr.hpp>

namespace Hord {
namespace Object {

// Forward declarations
enum class Type : unsigned;
struct type_info;
class Unit; // external

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
using ID = std::uint32_t;

/**
	Object::ID constants.
*/
enum : Object::ID {
	/**
		Null object.
	*/
	NULL_ID = static_cast<Object::ID>(0u)
};

/**
	%Object type.
*/
enum class Type : unsigned {
	/** @ref hive. */
	Hive = 0u,

	/** @ref rule. */
	Rule,

	/** @ref node. */
	Node,

/** @cond INTERNAL */
	LAST
/** @endcond */
};

/**
	Get the name of an object type.

	@returns C-string containing the name of @a object_type or
	"INVALID" if somehow @a object_type is not actually
	an @c Object::Type.
	@param object_type Object type.
*/
char const*
get_type_name(
	Object::Type const object_type
) noexcept;

/**
	Object type info.
*/
struct type_info final {
	/**
		Object type.

		@sa Object::Type
	*/
	Object::Type const type;

	/**
		Prop availability.
	*/
	IO::PropStateStore const props;
};

/**
	Owning pointer to object unit.
*/
using UPtr = duct::cc_unique_ptr<Object::Unit>;

/** @} */ // end of doc-group object

} // namespace Object
} // namespace Hord

#endif // HORD_OBJECT_DEFS_HPP_
