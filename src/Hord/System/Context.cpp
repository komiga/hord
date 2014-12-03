
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
IO::Datastore&
lookup_datastore(
	System::Driver& driver,
	IO::Datastore::ID const datastore_id
) {
	auto* const datastore = driver.find_datastore(datastore_id);
	if (nullptr == datastore) {
		HORD_THROW_FQN(
			ErrorCode::context_invalid_datastore,
			"datastore does not exist within driver"
		);
	}
	return *datastore;
}
#undef HORD_SCOPE_FUNC

Context::Context(
	System::Driver& driver,
	IO::Datastore& datastore
)
	: m_driver(driver)
	, m_datastore(datastore)
{}

Context::~Context() noexcept = default;

Context::Context(Context&&) noexcept = default;

Context::Context(
	System::Driver& driver,
	IO::Datastore::ID const datastore_id
)
	: Context(
		driver,
		lookup_datastore(driver, datastore_id)
	)
{}

#undef HORD_SCOPE_CLASS

} // namespace System
} // namespace Hord
