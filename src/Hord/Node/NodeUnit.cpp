
#include <Hord/Object/Defs.hpp>
#include <Hord/Node/Unit.hpp>

namespace Hord {
namespace Node {

// class Unit implementation

#define HORD_SCOPE_CLASS_IDENT__ Node::Unit

namespace {
static constexpr Object::type_info const
s_type_info{
	Object::Type::Node,
	true,
	true
};
} // anonymous namespace

Object::type_info const&
Unit::get_type_info_impl() const noexcept {
	return s_type_info;
}

Unit::Unit(
	Node::ID const id,
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

void
Unit::set_layout_ref(
	Node::ID const node_id
) noexcept {
	m_layout_ref = node_id;
	if (Object::NULL_ID != static_cast<Object::ID>(m_layout_ref)) {
		m_layout.clear();
	}
}

#define HORD_SCOPE_FUNC_IDENT__ deserialize_impl
void
Unit::deserialize_impl(
	IO::InputPropStream& /*prop_stream*/
) {
}
#undef HORD_SCOPE_FUNC_IDENT__

#define HORD_SCOPE_FUNC_IDENT__ serialize_impl
void
Unit::serialize_impl(
	IO::OutputPropStream& /*prop_stream*/
) const {
}
#undef HORD_SCOPE_FUNC_IDENT__

#undef HORD_SCOPE_CLASS_IDENT__

} // namespace Node
} // namespace Hord
