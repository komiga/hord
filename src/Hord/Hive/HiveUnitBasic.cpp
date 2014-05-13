
#include <Hord/serialization.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Hive/UnitBasic.hpp>

#include <Hord/detail/gr_ceformat.hpp>

namespace Hord {
namespace Hive {

// class UnitBasic implementation

#define HORD_SCOPE_CLASS Hive::UnitBasic

Object::UPtr
UnitBasic::construct(
	Object::ID const id,
	Object::ID const /*parent*/
) noexcept {
	return Object::UPtr{
		new(std::nothrow) Hive::UnitBasic(id)
	};
}

UnitBasic::~UnitBasic() noexcept = default;

UnitBasic::UnitBasic(UnitBasic&&) = default;
UnitBasic& UnitBasic::operator=(UnitBasic&&) = default;

UnitBasic::UnitBasic(
	Hive::ID const id
) noexcept
	: base(
		Hive::UnitBasic::info,
		static_cast<Object::ID>(id)
	)
{}

#undef HORD_SCOPE_CLASS

} // namespace Hive
} // namespace Hord
