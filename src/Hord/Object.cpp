
#include <Hord/Object.hpp>

namespace Hord {

// class Object implementation

Object::Object() = default;

Object::Object(
	StorageState const storage_state,
	ObjectID const owner,
	ObjectID const id
) noexcept
	: m_storage_state(storage_state)
	, m_owner(owner)
	, m_id(id)
{}

Object::Object(
	ObjectID const owner,
	ObjectID const id
) noexcept
	: m_owner(owner)
	, m_id(id)
{}

Object::Object(Object&&) = default;
Object::~Object() noexcept = default;

Object& Object::operator=(Object&&) = default;

void
Object::set_slug(
	String slug
) noexcept {
	m_slug.assign(std::move(slug));
	if (64u < m_slug.size()) {
		m_slug.resize(64u);
	}
	// TODO: Truncate invalid unit sequence (if any) after resize
}

} // namespace Hord
