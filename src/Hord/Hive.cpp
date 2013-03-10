
#include <Hord/Hive.hpp>

namespace Hord {

// class Hive implementation

namespace {
static constexpr Object::type_info s_type_info{
	ObjectType::Hive
};
} // anonymous namespace

Object::type_info const& Hive::get_type_info_impl() const noexcept {
	return s_type_info;
}

} // namespace Hord
