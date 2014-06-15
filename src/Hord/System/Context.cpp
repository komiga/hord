
#include <Hord/utility.hpp>
#include <Hord/Log.hpp>
#include <Hord/System/Driver.hpp>
#include <Hord/System/Context.hpp>

#include <cassert>

#include <Hord/detail/gr_core.hpp>

namespace Hord {
namespace System {

// class Context implementation

#define HORD_SCOPE_CLASS System::Context

#define HORD_SCOPE_FUNC ctor // pseudo
System::Driver::datastore_hive_pair&
get_hive_pair(
	System::Driver& driver,
	Hive::ID const hive_id
) {
	auto const it = driver.find_hive(hive_id);
	if (driver.get_hives().end() == it) {
		HORD_THROW_FQN(
			ErrorCode::context_invalid_hive,
			"hive ID does not exist within driver"
		);
	}
	return it->second;
}
#undef HORD_SCOPE_FUNC

Context::Context(
	System::Driver& driver,
	System::Driver::datastore_hive_pair& hive_pair
)
	: m_driver(driver)
	, m_datastore(*hive_pair.datastore)
	, m_hive(static_cast<Hive::Unit&>(*hive_pair.hive))
{}

Context::~Context() noexcept = default;

Context::Context(Context&&) noexcept = default;

Context::Context(
	System::Driver& driver,
	Hive::ID const hive_id
)
	: Context(
		driver,
		get_hive_pair(driver, hive_id)
	)
{}

#undef HORD_SCOPE_CLASS

} // namespace System
} // namespace Hord
