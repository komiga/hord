/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Table definitions.
@ingroup table
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/IO/PropStateStore.hpp>

namespace Hord {
namespace Table {

// Forward declarations
enum class UnitType : Object::TypeValue;

/**
	@addtogroup table
	@{
*/

/**
	Table ID.

	@note This type is only to telegraph intent in functions;
	it is entirely equivalent to @c Object::ID.

	@sa Object::ID
*/
using ID = Object::GenID<Object::BaseType::Table>;

static constexpr Table::ID const
/**
	Null ID.
*/
ID_NULL{Object::ID_NULL};

/**
	Standard table unit types.
*/
enum class UnitType : Object::TypeValue {
	/** Basic table unit. */
	Basic = 1u
};

} // namespace Table

/** @cond INTERNAL */
namespace Object {
template<>
struct unit_type_traits<Table::UnitType>
	: public Object::unit_type_traits_impl<
		Table::UnitType,
		Object::BaseType::Table
	>
{};
}
/** @endcond */ // INTERNAL

namespace Table {

/**
	Table object type.
*/
using Type = Object::GenType<Table::UnitType>;

/**
	Supplied props for table objects.
*/
static constexpr IO::PropStateStore const
SUPPLIED_PROPS{true, false};

/** @} */ // end of doc-group table

} // namespace Table
} // namespace Hord
