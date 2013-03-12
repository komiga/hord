
#include <Hord/Object.hpp>

namespace Hord {

// class Object implementation

Object::~Object()=default;

void Object::set_slug(String slug) noexcept {
	m_slug.assign(std::move(slug));
	if (64u<m_slug.size()) {
		m_slug.resize(64u);
	}
	// TODO: Truncate invalid unit sequence (if any) after resize
}

} // namespace Hord
