/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/utility.hpp>
#include <Hord/String.hpp>
#include <Hord/IO/Defs.hpp>

namespace Hord {
namespace IO {

namespace {
	static char const
	s_prop_type_invalid[]{HORD_STR_LIT("INVALID")},
	* const s_prop_type_names[]{
		HORD_STR_LIT("identity"),
		HORD_STR_LIT("metadata"),
		HORD_STR_LIT("scratch"),
		HORD_STR_LIT("primary"),
		HORD_STR_LIT("auxiliary"),
	};
} // anonymous namespace

static_assert(
	enum_cast(IO::PropType::LAST)
	== array_extent(s_prop_type_names),
	"PropType name list is incomplete"
);

char const*
get_prop_type_name(
	IO::PropType const prop_type
) noexcept {
	std::size_t const index = enum_cast(prop_type);
	if (index < array_extent(s_prop_type_names)) {
		return s_prop_type_names[index];
	} else {
		return s_prop_type_invalid;
	}
}

} // namespace IO
} // namespace Hord
