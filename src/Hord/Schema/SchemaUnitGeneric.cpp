/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/String.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Schema/UnitGeneric.hpp>

#include <duct/debug.hpp>

#include <Hord/detail/gr_ceformat.hpp>

namespace Hord {
namespace Schema {

// class UnitGeneric implementation

#define HORD_SCOPE_CLASS Schema::UnitGeneric

constexpr Object::TypeInfo const
Schema::UnitGeneric::info;

Object::UPtr
UnitGeneric::construct(
	Object::ID const id,
	Object::ID const parent
) noexcept {
	return Object::UPtr{
		new(std::nothrow) Schema::UnitGeneric(Schema::ID{id}, parent)
	};
}

UnitGeneric::~UnitGeneric() noexcept = default;

UnitGeneric::UnitGeneric(UnitGeneric&&) = default;
UnitGeneric& UnitGeneric::operator=(UnitGeneric&&) = default;

UnitGeneric::UnitGeneric(
	Schema::ID const id,
	Object::ID const parent
) noexcept
	: base(
		Schema::UnitGeneric::info,
		id,
		parent
	)
{}

void
UnitGeneric::deserialize_prop_auxiliary(
	IO::InputPropStream& /*prop_stream*/,
	InputSerializer& /*ser*/
) {
	DUCT_ASSERTE(false);
}

void
UnitGeneric::serialize_prop_auxiliary(
	IO::OutputPropStream& /*prop_stream*/,
	OutputSerializer& /*ser*/
) const {
	DUCT_ASSERTE(false);
}

#undef HORD_SCOPE_CLASS

} // namespace Schema
} // namespace Hord
