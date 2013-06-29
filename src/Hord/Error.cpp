
#include <Hord/ErrorCode.hpp>
#include <Hord/String.hpp>
#include <Hord/Error.hpp>

#include <type_traits>
#include <utility>

namespace Hord {

namespace {
static char const
	s_error_invalid[] = HORD_STR_LIT("INVALID"),
	* const s_error_names[]{
		HORD_STR_LIT("unknown"),

		HORD_STR_LIT("driver_rule_type_reserved"),
		HORD_STR_LIT("driver_rule_type_zero_permitted_types"),
		HORD_STR_LIT("driver_rule_type_shared"),
		HORD_STR_LIT("driver_hive_root_empty"),
		HORD_STR_LIT("driver_hive_root_shared"),
		HORD_STR_LIT("driver_datastore_construct_failed"),
		HORD_STR_LIT("driver_datastore_locked"),

		HORD_STR_LIT("datastore_open_already"),
		HORD_STR_LIT("datastore_open_failed"),
		HORD_STR_LIT("datastore_closed"),
		HORD_STR_LIT("datastore_locked"),
		HORD_STR_LIT("datastore_property_immutable"),
		HORD_STR_LIT("datastore_object_not_found"),
		HORD_STR_LIT("datastore_prop_unsupplied"),
		HORD_STR_LIT("datastore_prop_not_locked"),

		HORD_STR_LIT("serialization_improper_state"),
		HORD_STR_LIT("serialization_access"),
	}
;
} // anonymous namespace

static_assert(
	static_cast<std::size_t>(ErrorCode::LAST)
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

// class Error implementation

Error::Error(
	ErrorCode const errc,
	String msg
) noexcept
	: std::exception()
	, m_errc(errc)
	, m_msg(std::move(msg))
{}

Error::Error(Error&&) = default;
Error::~Error() noexcept = default;

} // namespace Hord
