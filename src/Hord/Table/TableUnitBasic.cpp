/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/String.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Table/UnitBasic.hpp>

#include <Hord/detail/gr_ceformat.hpp>

namespace Hord {
namespace Table {

// class UnitBasic implementation

#define HORD_SCOPE_CLASS Table::UnitBasic

constexpr Object::type_info const
Table::UnitBasic::info;

Object::UPtr
UnitBasic::construct(
	Object::ID const id,
	Object::ID const parent
) noexcept {
	return Object::UPtr{
		new(std::nothrow) Table::UnitBasic(Table::ID{id}, parent)
	};
}

UnitBasic::~UnitBasic() noexcept = default;

UnitBasic::UnitBasic(UnitBasic&&) = default;
UnitBasic& UnitBasic::operator=(UnitBasic&&) = default;

UnitBasic::UnitBasic(
	Table::ID const id,
	Object::ID const parent
) noexcept
	: base(
		Table::UnitBasic::info,
		id,
		parent
	)
{}

#undef HORD_SCOPE_CLASS

} // namespace Table
} // namespace Hord
