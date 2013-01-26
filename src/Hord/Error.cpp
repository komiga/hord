
#include <Hord/String.hpp>
#include <Hord/Error.hpp>

namespace Hord {

static char const g_error_invalid[]=HORD_STR_LIT("INVALID");
static char const* const g_error_names[]={
	HORD_STR_LIT("unknown"),
	HORD_STR_LIT("driver_hive_root_empty"),
	HORD_STR_LIT("driver_hive_root_shared"),
	HORD_STR_LIT("driver_hive_locked"),
	HORD_STR_LIT("serialization_improper_state"),
	HORD_STR_LIT("serialization_access")
};

char const* get_error_name(ErrorCode const error_code) noexcept {
	unsigned index=static_cast<unsigned>(error_code);
	if (index<sizeof(g_error_names)/sizeof(*g_error_names)) {
		return g_error_names[index];
	} else {
		return g_error_invalid;
	}
}

} // namespace Hord
