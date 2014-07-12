/**
@file Node/Defs.hpp
@brief Node definitions.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/Object/Defs.hpp>

#include <Hord/IO/PropStateStore.hpp>

namespace Hord {
namespace Node {

// Forward declarations
enum class UnitType : Object::TypeValue;

/**
	@addtogroup object
	@{
*/
/**
	@addtogroup node
	@{
*/

/**
	Node ID.

	@note This type is only to telegraph intent in functions;
	it is entirely equivalent to @c Object::ID.

	@sa Object::ID,
		Rule::ID,
		Hive::ID
*/
using ID = Object::GenID<Object::BaseType::Node>;

static constexpr Node::ID const
/**
	Null ID.
*/
ID_NULL{Object::ID_NULL};

/**
	Standard node unit types.
*/
enum class UnitType : Object::TypeValue {
	/** Basic node unit. */
	basic = 1u
};

} // namespace Node

/** @cond INTERNAL */
namespace Object {
template<>
struct unit_type_traits<Node::UnitType>
	: public Object::unit_type_traits_impl<
		Node::UnitType,
		Object::BaseType::Node
	>
{};
}
/** @endcond */ // INTERNAL

namespace Node {

/**
	Node object type.
*/
using Type = Object::GenType<Node::UnitType>;

/**
	Supplied props for node objects.
*/
static constexpr IO::PropStateStore const
SUPPLIED_PROPS{true, true};

/** @} */ // end of doc-group node
/** @} */ // end of doc-group object

} // namespace Node
} // namespace Hord
