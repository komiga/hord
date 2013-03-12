
#include <Hord/Node.hpp>

namespace Hord {

// class Node implementation

namespace {
static constexpr Object::type_info s_type_info{
	ObjectType::Node
};
} // anonymous namespace

Object::type_info const& Node::get_type_info_impl() const noexcept {
	return s_type_info;
}

void Node::set_layout_ref(NodeID const node_id) noexcept {
	m_layout_ref=node_id;
	if (OBJECT_NULL!=m_layout_ref) {
		m_layout.clear();
	}
}

} // namespace Hord
