
#include <Hord/Rule/Unit.hpp>

namespace Hord {
namespace Rule {

// class Unit implementation

#define HORD_SCOPE_CLASS Rule::Unit

Object::type_info const&
Unit::get_type_info_impl() const noexcept {
	return s_type_info;
}

Unit::~Unit() noexcept = default;

Unit::Unit(Unit&&) = default;
Unit& Unit::operator=(Unit&&) = default;

Unit::Unit(
	Rule::ID const id,
	Object::ID const parent
) noexcept
	: base(
		s_type_info,
		static_cast<Object::ID>(id),
		parent
	)
{}

#undef HORD_SCOPE_CLASS

} // namespace Rule
} // namespace Hord
