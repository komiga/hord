
#include <Hord/Object/Defs.hpp>
#include <Hord/Node/Unit.hpp>

namespace Hord {
namespace Node {

// class Unit implementation

#define HORD_SCOPE_CLASS_IDENT__ Node::Unit

namespace {
static Object::type_info const
s_type_info{
	Object::Type::Node
};
} // anonymous namespace

Object::type_info const&
Unit::get_type_info_impl() const noexcept {
	return s_type_info;
}

Unit::Unit(
	Hive::ID const owner,
	Node::ID const id
) noexcept
	: base(
		(Object::NULL_ID == id)
			? IO::StorageState::null
			: IO::StorageState::placeholder
		, static_cast<Object::ID>(owner)
		, static_cast<Object::ID>(id)
	)
{}

Unit::Unit(Unit&&) = default;
Unit::~Unit() noexcept = default;

Unit& Unit::operator=(Unit&&) = default;

void
Unit::set_layout_ref(
	Node::ID const node_id
) noexcept {
	m_layout_ref = node_id;
	if (Object::NULL_ID != static_cast<Object::ID>(m_layout_ref)) {
		m_layout.clear();
	}
}

#undef HORD_SCOPE_CLASS_IDENT__

} // namespace Node
} // namespace Hord
