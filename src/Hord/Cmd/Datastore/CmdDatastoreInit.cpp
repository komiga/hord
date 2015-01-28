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
#include <Hord/Object/Ops.hpp>
#include <Hord/Cmd/Datastore.hpp>
#include <Hord/System/Driver.hpp>
#include <Hord/System/Context.hpp>

#include <duct/IO/memstream.hpp>

#include <exception>

#include <Hord/detail/gr_core.hpp>

namespace Hord {
namespace Cmd {
namespace Datastore {

#define HORD_SCOPE_CLASS Cmd::Datastore::Init

#define HORD_SCOPE_FUNC link_parents
void
link_parents(
	IO::Datastore& datastore
) {
	auto& objects = datastore.objects();
	for (auto& obj_pair : objects) {
		auto& obj = *obj_pair.second;
		auto parent = obj.parent();
		if (obj.id() == parent) {
			Log::acquire(Log::error)
				<< "Object " << obj << " has itself as parent\n"
			;
			parent = Object::ID_NULL;
		} else if (
			Object::ID_NULL != obj.parent() &&
			!datastore.has_object(obj.parent())
		) {
			Log::acquire(Log::error)
				<< "Object " << obj << " points to invalid parent: "
				<< Object::IDPrinter{obj.parent()}
				<< "\n"
			;
			parent = Object::ID_NULL;
		}
		// NB: Ignoring circular; client should decide what to do
		// with these objects
		Object::set_parent(obj, datastore, parent);
		if (obj.parent() == Object::ID_NULL) {
			datastore.root_objects().emplace(obj.id());
		}
	}
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC exec // pseudo
HORD_SCOPE_CLASS::exec_result_type
HORD_SCOPE_CLASS::operator()(
	IO::PropTypeBit prop_types
) noexcept try {
	auto const& driver = this->driver();
	auto& datastore = this->datastore();
	if (datastore.is_initialized()) {
		Log::acquire()
			<< DUCT_GR_MSG_FQN("datastore already initialized")
		;
		return commit_with(Cmd::Result::success_no_action);
	}

	// Identity must be loaded
	prop_types |= IO::PropTypeBit::identity;

	// Create and initialize resident objects
	auto const& si_map = make_const(datastore).storage_info();
	for (auto const& si_pair : si_map) {
		auto const& sinfo = si_pair.second;
		if (IO::Linkage::resident != sinfo.linkage) {
			continue;
		}
		auto const* const
		tinfo = driver.find_object_type_info(sinfo.object_type);
		if (nullptr == tinfo) {
			return commit_error("object type unknown");
		}
		auto const emplace_pair = datastore.objects().emplace(
			sinfo.object_id,
			tinfo->construct(
				sinfo.object_id,
				Object::ID_NULL
			)
		);
		if (!emplace_pair.second) {
			return commit_error("object already exists");
		} else if (nullptr == emplace_pair.first->second) {
			return commit_error("object allocation failed");
		}
		auto& obj = *emplace_pair.first->second;
		Cmd::Datastore::Load::load_or_initialize(
			datastore, obj, sinfo, prop_types
		);
	}
	link_parents(datastore);

	datastore.enable_state(IO::Datastore::State::initialized);
	return commit();
} catch (Error const& err) {
	notify_exception_current();
	switch (err.code()) {
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
