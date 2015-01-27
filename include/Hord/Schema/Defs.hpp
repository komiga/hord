/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Schema definitions.
@ingroup schema
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/Object/Defs.hpp>

namespace Hord {
namespace Schema {

// Forward declarations
enum class UnitType : Object::TypeValue;

/**
	@addtogroup schema
	@{
*/

/**
	Schema ID.

	@note This type is only to telegraph intent in functions;
	it is entirely equivalent to @c Object::ID.

	@sa Object::ID
*/
using ID = Object::GenID<Object::BaseType::Schema>;

static constexpr Schema::ID const
/**
	Null ID.
*/
ID_NULL{Object::ID_NULL};

/**
	Standard schema unit types.
*/
enum class UnitType : Object::TypeValue {
	/** Generic schema unit. */
	Generic = 1u,
	/** Rule schema unit. */
	Rule,
	/** Table schema unit. */
	Table,
};

} // namespace Schema

/** @cond INTERNAL */
namespace Object {
template<>
struct unit_type_traits<Schema::UnitType>
	: public Object::unit_type_traits_impl<
		Schema::UnitType,
		Object::BaseType::Schema
	>
{};
}
/** @endcond */ // INTERNAL

namespace Schema {

/**
	Schema object type.
*/
using Type = Object::GenType<Schema::UnitType>;

/** @} */ // end of doc-group schema

} // namespace Schema
} // namespace Hord
