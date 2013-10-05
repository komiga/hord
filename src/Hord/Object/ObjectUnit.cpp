
#include <Hord/Object/Unit.hpp>

namespace Hord {
namespace Object {

// class Unit implementation

#define HORD_SCOPE_CLASS_IDENT__ Object::Unit

Unit::Unit(
	bool const supplies_primary,
	bool const supplies_auxiliary,
	Object::ID const owner,
	Object::ID const id
) noexcept
	: m_prop_states(
		supplies_primary,
		supplies_auxiliary
	)
	, m_owner(owner)
	, m_id(id)
{}

Unit::Unit(Unit&&) = default;
Unit::~Unit() noexcept = default;

Unit& Unit::operator=(Unit&&) = default;

void
Unit::set_slug(
	String slug
) noexcept {
	m_slug.assign(std::move(slug));
	if (64u < m_slug.size()) {
		m_slug.resize(64u);
	}
	// TODO: Truncate invalid unit sequence (if any) after resize
}

#undef HORD_SCOPE_CLASS_IDENT__

} // namespace Object
} // namespace Hord
