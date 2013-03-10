
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

} // namespace Hord
