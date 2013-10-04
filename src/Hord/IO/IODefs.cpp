
#include <Hord/String.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/Prop.hpp>

#include <type_traits>
#include <utility>

namespace Hord {
namespace IO {

namespace {
static char const
s_prop_type_invalid[]{HORD_STR_LIT("INVALID")},
* const s_prop_type_names[]{
	HORD_STR_LIT("object_metadata"),
	HORD_STR_LIT("object_primary"),
	HORD_STR_LIT("object_auxiliary"),
	HORD_STR_LIT("object_scratch")
};
} // anonymous namespace

static_assert(
	static_cast<std::size_t>(IO::PropType::LAST)
	== std::extent<decltype(s_prop_type_names)>::value,
	"PropType name list is incomplete"
);

char const*
get_prop_type_name(
	IO::PropType const prop_type
) noexcept {
	std::size_t const index = static_cast<std::size_t>(prop_type);
	if (index < std::extent<decltype(s_prop_type_names)>::value) {
		return s_prop_type_names[index];
	} else {
		return s_prop_type_invalid;
	}
}

} // namespace IO
} // namespace Hord
