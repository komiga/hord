
#include <Hord/utility.hpp>
#include <Hord/ErrorCode.hpp>
#include <Hord/String.hpp>
#include <Hord/Error.hpp>

#include <type_traits>
#include <utility>

namespace Hord {

namespace {
static char const
s_error_invalid[]{HORD_STR_LIT("INVALID")},
* const s_error_names[]{
	HORD_STR_LIT("unknown"),

// lockfile
	HORD_STR_LIT("lockfile_immutable"),
	HORD_STR_LIT("lockfile_acquire_failed"),

// driver
	HORD_STR_LIT("driver_rule_type_reserved"),
	HORD_STR_LIT("driver_rule_type_zero_permitted_types"),
	HORD_STR_LIT("driver_rule_type_shared"),
	HORD_STR_LIT("driver_command_table_range_invalid"),
	HORD_STR_LIT("driver_command_table_range_shared"),
	HORD_STR_LIT("driver_hive_root_empty"),
	HORD_STR_LIT("driver_hive_root_shared"),
	HORD_STR_LIT("driver_datastore_construct_failed"),
	HORD_STR_LIT("driver_datastore_locked"),

// datastore
	HORD_STR_LIT("datastore_open_already"),
	HORD_STR_LIT("datastore_open_failed"),
	HORD_STR_LIT("datastore_closed"),
	HORD_STR_LIT("datastore_locked"),
	HORD_STR_LIT("datastore_property_immutable"),

	HORD_STR_LIT("datastore_object_type_prohibited"),
	HORD_STR_LIT("datastore_object_not_found"),
	HORD_STR_LIT("datastore_object_already_exists"),

	HORD_STR_LIT("datastore_prop_unsupplied"),
	HORD_STR_LIT("datastore_prop_void"),
	HORD_STR_LIT("datastore_prop_not_locked"),

// serialization
	HORD_STR_LIT("serialization_prop_unsupplied"),
	HORD_STR_LIT("serialization_prop_improper_state"),
	HORD_STR_LIT("serialization_io_failed"),
	HORD_STR_LIT("serialization_data_malformed"),

// cmd
	HORD_STR_LIT("cmd_construct_stage_type_invalid"),

// context
	HORD_STR_LIT("context_invalid_hive"),

	HORD_STR_LIT("context_output_detached"),
	HORD_STR_LIT("context_execute_not_active"),
	HORD_STR_LIT("context_execute_already_active"),
};
} // anonymous namespace

static_assert(
	enum_cast(ErrorCode::LAST)
	== std::extent<decltype(s_error_names)>::value,
	"ErrorCode name list is incomplete"
);

char const*
get_error_name(
	ErrorCode const error_code
) noexcept {
	std::size_t const index = static_cast<std::size_t>(error_code);
	if (index < std::extent<decltype(s_error_names)>::value) {
		return s_error_names[index];
	} else {
		return s_error_invalid;
	}
}

} // namespace Hord
