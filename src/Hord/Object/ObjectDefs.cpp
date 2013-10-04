
#include <Hord/String.hpp>
#include <Hord/Object/Defs.hpp>

#include <type_traits>
#include <utility>

namespace Hord {
namespace Object {

namespace {
static char const
s_object_type_invalid[]{HORD_STR_LIT("INVALID")},
* const s_object_type_names[]{
	HORD_STR_LIT("Hive"),
	HORD_STR_LIT("Rule"),
	HORD_STR_LIT("Node")
};
} // anonymous namespace

static_assert(
	static_cast<std::size_t>(Object::Type::LAST)
	== std::extent<decltype(s_object_type_names)>::value,
	"Object::Type name list is incomplete"
);

char const*
get_type_name(
	Object::Type const object_type
) noexcept {
	std::size_t const index = static_cast<std::size_t>(object_type);
	if (index < std::extent<decltype(s_object_type_names)>::value) {
		return s_object_type_names[index];
	} else {
		return s_object_type_invalid;
	}
}

} // namespace Object
} // namespace Hord
