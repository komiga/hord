
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
	auto& objects = datastore.get_objects();
	for (auto& obj_pair : objects) {
		auto& obj = *obj_pair.second;
		auto parent = obj.get_parent();
		if (obj.get_id() == parent) {
			Log::acquire(Log::error)
				<< "Object " << obj << " has itself as parent\n"
			;
			parent = Object::ID_NULL;
		} else if (!datastore.has_object(obj.get_parent())) {
			Log::acquire(Log::error)
				<< "Object " << obj << " points to invalid parent: "
				<< Object::IDPrinter{obj.get_parent()}
				<< "\n"
			;
			parent = Object::ID_NULL;
		}
		// NB: Ignoring circular; client should decide what to do
		// with these objects
		Object::set_parent(obj, datastore, parent);
	}
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC init_object
void
init_object(
	IO::Datastore& datastore,
	Object::Unit& obj,
	IO::StorageInfo const& sinfo,
	IO::PropTypeBit prop_types
) {
	prop_types = obj.get_prop_states().supplied_of(prop_types);
	IO::load_props(
		datastore,
		obj,
		sinfo.prop_storage.initialized_of(prop_types)
	);

	// Initialize any base props requested that were not stored.
	// Can't do anything about primary and auxiliary props.
	auto const unstored_types
	= sinfo.prop_storage.uninitialized_of(static_cast<IO::PropTypeBit>(
		enum_bitand(IO::PropTypeBit::base, prop_types)
	));
	if (enum_bitand(IO::PropTypeBit::identity, unstored_types)) {
		enum : std::size_t {
			BUF_SIZE = 8u
		};
		char ms_buf[BUF_SIZE];
		duct::IO::omemstream ms{ms_buf, BUF_SIZE};
		ms << Object::IDPrinter{obj.get_id()};
		obj.set_slug(String{ms_buf, BUF_SIZE});
		obj.set_parent(Object::ID_NULL);
		obj.get_prop_states().assign(
			IO::PropType::identity,
			IO::PropState::modified
		);
	}
	if (enum_bitand(IO::PropTypeBit::metadata, unstored_types)) {
		// Metadata collection should already be empty; no base
		// values to add
		obj.get_prop_states().assign(
			IO::PropType::metadata,
			IO::PropState::modified
		);
	}
	if (enum_bitand(IO::PropTypeBit::scratch, unstored_types)) {
		// Scratch space should also be empty
		obj.get_prop_states().assign(
			IO::PropType::scratch,
			IO::PropState::modified
		);
	}
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC exec // pseudo
HORD_SCOPE_CLASS::exec_result_type
HORD_SCOPE_CLASS::operator()(
	IO::PropTypeBit prop_types
) noexcept try {
	auto const& driver = get_driver();
	auto& datastore = get_datastore();
	if (datastore.is_initialized()) {
		Log::acquire()
			<< DUCT_GR_MSG_FQN("datastore already initialized")
		;
		return commit_with(Cmd::Result::success_no_action);
	}

	// Identity must be loaded
	prop_types = enum_combine(
		IO::PropTypeBit::identity,
		prop_types
	);

	// Create and initialize resident objects
	auto const& si_map = make_const(datastore).get_storage_info();
	for (auto const& si_pair : si_map) {
		auto const& sinfo = si_pair.second;
		if (IO::Linkage::resident != sinfo.linkage) {
			continue;
		}
		auto const* const
		tinfo = driver.get_object_type_info(sinfo.object_type);
		if (nullptr == tinfo) {
			return commit_error("object type unknown");
		}
		auto const emplace_pair = datastore.get_objects().emplace(
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
		init_object(datastore, obj, sinfo, prop_types);
	}
	link_parents(datastore);

	datastore.enable_state(IO::Datastore::State::initialized);
	return commit();
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
