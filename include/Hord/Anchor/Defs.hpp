/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Anchor definitions.
@ingroup anchor
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/Object/Defs.hpp>

#include <Hord/IO/PropStateStore.hpp>

namespace Hord {
namespace Anchor {

// Forward declarations
enum class UnitType : Object::TypeValue;

/**
	@addtogroup anchor
	@{
*/

/**
	Anchor ID.

	@sa Object::ID
*/
using ID = Object::GenID<Object::BaseType::Anchor>;

static constexpr Anchor::ID const
/**
	Null ID.
*/
ID_NULL{Object::ID_NULL};

/**
	Standard anchor unit types.
*/
enum class UnitType : Object::TypeValue {
	/** Typeless. */
	null = 0u,
	/** Basic anchor unit. */
	Basic = 1u
};

} // namespace Anchor

/** @cond INTERNAL */
namespace Object {
template<>
struct unit_type_traits<Anchor::UnitType>
	: public Object::unit_type_traits_impl<
		Anchor::UnitType,
		Object::BaseType::Anchor
	>
{};
}
/** @endcond */ // INTERNAL

namespace Anchor {

/**
	Anchor object type.
*/
using Type = Object::GenType<Anchor::UnitType>;

/**
	Supplied props for anchor objects.
*/
static constexpr IO::PropStateStore const
SUPPLIED_PROPS{false, false};

/** @} */ // end of doc-group anchor

} // namespace Anchor
} // namespace Hord
