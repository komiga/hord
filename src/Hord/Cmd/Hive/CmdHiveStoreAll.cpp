
#include <Hord/aux.hpp>
#include <Hord/utility.hpp>
#include <Hord/Log.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/Prop.hpp>
#include <Hord/IO/Datastore.hpp>
#include <Hord/IO/Ops.hpp>
#include <Hord/Hive/Defs.hpp>
#include <Hord/Hive/Unit.hpp>
#include <Hord/Cmd/Hive.hpp>
#include <Hord/System/Driver.hpp>
#include <Hord/System/Context.hpp>

#include <Hord/detail/gr_core.hpp>

namespace Hord {
namespace Cmd {
namespace Hive {

#define HORD_SCOPE_CLASS Cmd::Hive::StoreAll

#define HORD_SCOPE_FUNC 
static void
store_object(
	IO::Datastore& datastore,
	Hord::Object::Unit& object,
	unsigned& num_props_stored,
	unsigned& num_objects_stored
) {
	unsigned const num_stored = IO::store_props(
		datastore,
		object,
		object.get_prop_states().get_supplied()
	);
	num_props_stored += num_stored;
	num_objects_stored += static_cast<unsigned>(0 < num_stored);
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC exec // pseudo
HORD_SCOPE_CLASS::exec_result_type
HORD_SCOPE_CLASS::operator()() noexcept try {
	auto& datastore = get_datastore();
	auto& hive = get_hive();

	store_object(
		datastore,
		hive,
		m_num_props_stored,
		m_num_objects_stored
	);
	for (auto& obj_pair : hive.get_objects()) {
		store_object(
			datastore,
			*obj_pair.second, 
			m_num_props_stored,
			m_num_objects_stored
		);
	}
	return commit();
} catch (Error const& err) {
	Log::acquire(Log::error)
		<< DUCT_GR_MSG_FQN("error storing props:\n")
	;
	Log::report_error(err);
	switch (err.get_code()) {
	case ErrorCode::serialization_prop_improper_state: // fall-through
	case ErrorCode::serialization_io_failed:
		return commit("serialization error");

	default:
		return commit("unknown error");
	}
}
#undef HORD_SCOPE_FUNC

} // namespace Hive
} // namespace Cmd
} // namespace Hord
