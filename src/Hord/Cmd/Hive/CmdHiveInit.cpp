
#include <Hord/aux.hpp>
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

#include <duct/IO/memstream.hpp>

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
		sinfo.prop_storage.supplied_of(prop_types)
	);

	// Initialize any base props requested that were not stored.
	// Can't do anything about primary and auxiliary props.
	auto const unsupplied
	= sinfo.prop_storage.unsupplied_of(static_cast<IO::PropTypeBit>(
		enum_bitand(IO::PropTypeBit::base, prop_types)
	));
	if (enum_bitand(IO::PropTypeBit::identity, unsupplied)) {
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
	if (enum_bitand(IO::PropTypeBit::metadata, unsupplied)) {
		// Metadata collection should already be empty; no base
		// values to add
		obj.get_prop_states().assign(
			IO::PropType::metadata,
			IO::PropState::modified
		);
	}
	if (enum_bitand(IO::PropTypeBit::scratch, unsupplied)) {
		// Scratch space should also be empty
		obj.get_prop_states().assign(
			IO::PropType::scratch,
			IO::PropState::modified
		);
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

	auto const& si_map = make_const(datastore).get_storage_info();
	{
	auto it_hive = si_map.find(Object::ID_HIVE);
	if (si_map.cend() == it_hive) {
		// If there is no storage info for the hive, the datastore
		// must be new. We have to setup the storage info here
		// because the datastore doesn't know anything about the
		// hive object, and thus none of its type info.
		it_hive = datastore.create_object(
			Object::ID_HIVE,
			hive.get_type_info(),
			IO::Linkage::resident
		);
	}
	init_object(datastore, hive, it_hive->second, prop_types);
	}

	for (auto const& si_pair : si_map) {
		auto const& sinfo = si_pair.second;
		if (
			IO::Linkage::resident != sinfo.linkage ||
			Object::ID_HIVE == sinfo.object_id
		) {
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
		init_object(datastore, obj, sinfo, prop_types);
	}
	link_parents(hive);
	return commit();
} catch (Error const& err) {
	Log::acquire(Log::error)
		<< DUCT_GR_MSG_FQN("error storing prop:\n")
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
