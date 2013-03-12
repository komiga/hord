
#include <Hord/String.hpp>
#include <Hord/Error.hpp>

#include <type_traits>

namespace Hord {

namespace {
static char const
	s_error_invalid[]=HORD_STR_LIT("INVALID"),
	* const s_error_names[]{
		HORD_STR_LIT("unknown"),
		HORD_STR_LIT("mutate_hive_root_empty"),
		HORD_STR_LIT("driver_hive_root_empty"),
		HORD_STR_LIT("driver_hive_root_shared"),
		HORD_STR_LIT("driver_hive_locked"),
		HORD_STR_LIT("serialization_improper_state"),
		HORD_STR_LIT("serialization_access")
	}
;
} // anonymous namespace

char const* get_error_name(ErrorCode const error_code) noexcept {
	std::size_t const index=static_cast<std::size_t>(error_code);
	if (index<std::extent<decltype(s_error_names)>::value) {
		return s_error_names[index];
	} else {
		return s_error_invalid;
	}
}

} // namespace Hord
