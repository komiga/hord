
#include <Hord/utility.hpp>
#include <Hord/Log.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/Prop.hpp>
#include <Hord/IO/Datastore.hpp>
#include <Hord/IO/Ops.hpp>
#include <Hord/Object/Ops.hpp>
#include <Hord/Hive/Defs.hpp>
#include <Hord/Hive/Unit.hpp>
#include <Hord/Cmd/Hive.hpp>
#include <Hord/System/Driver.hpp>
#include <Hord/System/Context.hpp>

#include <exception>

#include <Hord/detail/gr_core.hpp>

namespace Hord {
namespace Cmd {
namespace Hive {

#define HORD_SCOPE_CLASS Cmd::Hive::Init

#define HORD_SCOPE_FUNC link_parents
void
link_parents(
	Hord::Hive::Unit& hive
) {
	auto& objects = hive.get_objects();
	for (auto& obj_pair : objects) {
		auto& obj = *obj_pair.second;
		auto parent = obj.get_parent();
		if (obj.get_id() == parent) {
			Log::acquire(Log::error)
				<< "Object " << obj << " has itself as parent\n"
			;
			parent = Object::ID_HIVE;
		} else if (
			Object::ID_HIVE != obj.get_parent() && (
				Object::ID_NULL == obj.get_parent() ||
				!hive.has_object(obj.get_parent())
			)
		) {
			Log::acquire(Log::error)
				<< "Object " << obj << " points to invalid parent: "
				<< Object::IDPrinter{obj.get_parent()}
				<< "\n"
			;
			parent = Object::ID_HIVE;
		}
		// NB: Ignoring circular & invalid parenting; client should
		// decide what to do with these objects
		Object::set_parent(obj, hive, parent);
	}
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC exec // pseudo
HORD_SCOPE_CLASS::result_type
HORD_SCOPE_CLASS::operator()(
	IO::PropTypeBit prop_types
) noexcept try {
	auto const& driver = get_driver();
	auto& datastore = get_datastore();
	auto& hive = get_hive();

	if (hive.get_prop_states().is_initialized(
		IO::PropType::identity
	)) {
		Log::acquire()
			<< DUCT_GR_MSG_FQN("hive already initialized")
		;
		return commit();
	}

	prop_types = enum_combine(
		IO::PropTypeBit::identity,
		prop_types
	);

	// NB: Errors from HiveInit should be treated as fatal, so
	// not worrying about resetting the hive state if an error
	// occurs
	IO::load_props(
		datastore, hive,
		hive.get_prop_states().supplied_conj(prop_types)
	);
	auto const& si_map = make_const(datastore).get_storage_info();
	for (auto const& si_pair : si_map) {
		auto const& sinfo = si_pair.second;
		if (IO::Linkage::resident != sinfo.linkage) {
			continue;
		}
		auto const* const
		tinfo = driver.get_object_type_info(sinfo.object_type);
		if (nullptr == tinfo) {
			return commit("object type unknown");
		}
		auto const emplace_pair = hive.get_objects().emplace(
			sinfo.object_id,
			tinfo->construct(
				sinfo.object_id,
				Object::ID_NULL
			)
		);
		if (!emplace_pair.second) {
			return commit("object already exists");
		} else if (nullptr == emplace_pair.first->second) {
			return commit("object allocation failed");
		}
		auto& obj = *emplace_pair.first->second;
		IO::load_props(
			datastore, obj,
			obj.get_prop_states().supplied_conj(prop_types)
		);
	}
	link_parents(hive);
	return commit();
} catch (Error const& err) {
	Log::acquire(Log::error)
		<< DUCT_GR_MSG_FQN("error storing prop:\n")
	;
	Log::report_error(std::current_exception());
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
