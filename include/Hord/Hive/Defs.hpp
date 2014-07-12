/**
@file Hive/Defs.hpp
@brief Hive definitions.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/Object/Defs.hpp>

#include <Hord/IO/PropStateStore.hpp>

namespace Hord {
namespace Hive {

// Forward declarations
enum class UnitType : Object::TypeValue;

/**
	@addtogroup object
	@{
*/
/**
	@addtogroup hive
	@{
*/

/**
	Hive ID.

	@sa Object::ID,
		Rule::ID,
		Node::ID
*/
using ID = Object::GenID<Object::BaseType::Hive>;

static constexpr Hive::ID const
/**
	Null ID.
*/
ID_NULL{Object::ID_NULL};

/**
	Standard hive unit types.
*/
enum class UnitType : Object::TypeValue {
	/** Typeless. */
	null = 0u,
	/** Basic hive unit. */
	basic = 1u
};

} // namespace Hive

/** @cond INTERNAL */
namespace Object {
template<>
struct unit_type_traits<Hive::UnitType>
	: public Object::unit_type_traits_impl<
		Hive::UnitType,
		Object::BaseType::Hive
	>
{};
}
/** @endcond */ // INTERNAL

namespace Hive {

/**
	Hive object type.
*/
using Type = Object::GenType<Hive::UnitType>;

/**
	Supplied props for hive objects.
*/
static constexpr IO::PropStateStore const
SUPPLIED_PROPS{false, false};

/** @} */ // end of doc-group hive
/** @} */ // end of doc-group object

} // namespace Hive
} // namespace Hord
