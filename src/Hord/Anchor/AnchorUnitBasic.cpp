/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/Object/Defs.hpp>
#include <Hord/Anchor/UnitBasic.hpp>

#include <Hord/detail/gr_ceformat.hpp>

namespace Hord {
namespace Anchor {

// class UnitBasic implementation

#define HORD_SCOPE_CLASS Anchor::UnitBasic

constexpr Object::type_info const
Anchor::UnitBasic::info;

Object::UPtr
UnitBasic::construct(
	Object::ID const id,
	Object::ID const parent
) noexcept {
	return Object::UPtr{
		new(std::nothrow) Anchor::UnitBasic(Anchor::ID{id}, parent)
	};
}

UnitBasic::~UnitBasic() noexcept = default;

UnitBasic::UnitBasic(UnitBasic&&) = default;
UnitBasic& UnitBasic::operator=(UnitBasic&&) = default;

UnitBasic::UnitBasic(
	Anchor::ID const id,
	Object::ID const parent
) noexcept
	: base(
		Anchor::UnitBasic::info,
		id,
		parent
	)
{}

#undef HORD_SCOPE_CLASS

} // namespace Anchor
} // namespace Hord
