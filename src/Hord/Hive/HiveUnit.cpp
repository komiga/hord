
#include <Hord/Object/Defs.hpp>
#include <Hord/Hive/Unit.hpp>

namespace Hord {
namespace Hive {

// class Unit implementation

#define HORD_SCOPE_CLASS_IDENT__ Hive::Unit

namespace {
static Object::type_info const
s_type_info{
	Object::Type::Hive
};
} // anonymous namespace

Object::type_info const&
Unit::get_type_info_impl() const noexcept {
	return s_type_info;
}

Unit::Unit() = default;

Unit::Unit(
	Hive::ID const id
) noexcept
	: base(
		(Object::NULL_ID == id)
			? IO::StorageState::null
			: IO::StorageState::placeholder
		, Object::NULL_ID
		, static_cast<Object::ID>(id)
	)
{}

Unit::Unit(Unit&&) = default;
Unit::~Unit() noexcept = default;

Unit& Unit::operator=(Unit&&) = default;

bool
Unit::has_child(
	Object::ID const id
) const noexcept(noexcept(m_idset.find(id))) {
	return m_idset.cend() != m_idset.find(id);
}

#undef HORD_SCOPE_CLASS_IDENT__

} // namespace Hive
} // namespace Hord
