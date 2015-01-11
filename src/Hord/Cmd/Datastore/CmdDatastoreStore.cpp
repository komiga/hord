/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/aux.hpp>
#include <Hord/utility.hpp>
#include <Hord/Log.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/Prop.hpp>
#include <Hord/IO/Datastore.hpp>
#include <Hord/IO/Ops.hpp>
#include <Hord/Cmd/Datastore.hpp>
#include <Hord/System/Driver.hpp>
#include <Hord/System/Context.hpp>

#include <Hord/detail/gr_core.hpp>

namespace Hord {
namespace Cmd {
namespace Datastore {

#define HORD_SCOPE_CLASS Cmd::Datastore::Store

#define HORD_SCOPE_FUNC store_object
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

#define HORD_SCOPE_FUNC exec_all // pseudo
HORD_SCOPE_CLASS::exec_result_type
HORD_SCOPE_CLASS::operator()() noexcept try {
	auto& datastore = get_datastore();
	for (auto& obj_pair : datastore.get_objects()) {
		store_object(
			datastore,
			*obj_pair.second,
			m_num_props_stored,
			m_num_objects_stored
		);
	}
	return commit_with(
		0 == m_num_objects_stored
		? Cmd::Result::success_no_action
		: Cmd::Result::success
	);
} catch (Error const& err) {
	notify_exception_current();
	switch (err.get_code()) {
	case ErrorCode::serialization_prop_improper_state: // fall-through
	case ErrorCode::serialization_io_failed:
		return commit_error("serialization error");

	default:
		return commit_error("unknown error");
	}
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC exec_single // pseudo
HORD_SCOPE_CLASS::exec_result_type
HORD_SCOPE_CLASS::operator()(
	Hord::Object::ID const object_id
) noexcept try {
	auto& datastore = get_datastore();
	auto* const object = datastore.find_ptr(object_id);
	if (!object) {
		return commit_error("object does not exist");
	}
	store_object(
		datastore,
		*object,
		m_num_props_stored,
		m_num_objects_stored
	);
	return commit_with(
		0 == m_num_objects_stored
		? Cmd::Result::success_no_action
		: Cmd::Result::success
	);
} catch (Error const& err) {
	notify_exception_current();
	switch (err.get_code()) {
	case ErrorCode::serialization_prop_improper_state: // fall-through
	case ErrorCode::serialization_io_failed:
		return commit_error("serialization error");

	default:
		return commit_error("unknown error");
	}
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS

} // namespace Datastore
} // namespace Cmd
} // namespace Hord
