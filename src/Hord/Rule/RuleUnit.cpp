
#include <Hord/Rule/Unit.hpp>

namespace Hord {
namespace Rule {

// class Unit implementation

#define HORD_SCOPE_CLASS_IDENT__ Rule::Unit

namespace {
static constexpr Object::type_info const
s_type_info{
	Object::Type::Rule,
	true,
	false
};
} // anonymous namespace

Object::type_info const&
Unit::get_type_info_impl() const noexcept {
	return s_type_info;
}

Unit::Unit(
	Rule::ID const id,
	Object::ID const parent
) noexcept
	: base(
		s_type_info.supplies_prop_primary,
		s_type_info.supplies_prop_auxiliary,
		static_cast<Object::ID>(id),
		parent
	)
{}

Unit::Unit(Unit&&) = default;
Unit::~Unit() noexcept = default;

Unit& Unit::operator=(Unit&&) = default;

#undef HORD_SCOPE_CLASS_IDENT__

} // namespace Rule
} // namespace Hord
