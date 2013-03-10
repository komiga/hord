
#include <Hord/Rule.hpp>

namespace Hord {

// class Rule implementation

namespace {
static constexpr Object::type_info s_type_info{
	ObjectType::Rule
};
} // anonymous namespace

Object::type_info const& Rule::get_type_info_impl() const noexcept {
	return s_type_info;
}

} // namespace Hord
