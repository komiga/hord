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

#include <duct/debug.hpp>

#include <Hord/detail/gr_core.hpp>

namespace Hord {
namespace Cmd {
namespace Datastore {

#define HORD_SCOPE_CLASS Cmd::Datastore::Load

#define HORD_SCOPE_FUNC load_or_initialize
unsigned
HORD_SCOPE_CLASS::load_or_initialize(
	IO::Datastore& datastore,
	Hord::Object::Unit& object,
	IO::StorageInfo const& sinfo,
	IO::PropTypeBit prop_types
) {
	prop_types = object.prop_states().supplied_of(prop_types);
	unsigned const num_props_loaded = IO::load_props(
		datastore,
		object,
		sinfo.prop_storage.initialized_of(prop_types)
	);
	// Initialize any base props requested that were not in storage.
	// Metadata and scratch space should already be empty.
	// Object root-ness not managed herein; invariants through commands
	// (firstly Cmd::Datastore::Init, later Cmd::Object::SetParent)
	// will ensure it is already accurate.
	auto const unstored_types = sinfo.prop_storage.uninitialized_of(prop_types);
	if (enum_cast(unstored_types & IO::PropTypeBit::identity)) {
		enum : std::size_t {
			BUF_SIZE = 16u
		};
		char ms_buf[BUF_SIZE];
		duct::IO::omemstream ms{ms_buf, BUF_SIZE};
		ms << Object::IDPrinter{object.id()};
		object.set_slug(String{ms_buf, BUF_SIZE});
		object.set_parent(Object::ID_NULL);
	}
	object.prop_states().assign(unstored_types, IO::PropState::modified);
	return num_props_loaded;
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC exec_all // pseudo
HORD_SCOPE_CLASS::exec_result_type
HORD_SCOPE_CLASS::operator()(
	IO::PropTypeBit prop_types
) noexcept try {
	auto& datastore = this->datastore();
	auto const& si_map = make_const(datastore).storage_info();

	auto it_sinfo = si_map.cend();
	unsigned num_props_loaded;
	for (auto& obj_pair : datastore.objects()) {
		auto& object = *obj_pair.second;
		it_sinfo = si_map.find(object.id());
		DUCT_ASSERTE(it_sinfo != si_map.cend());
		auto const& sinfo = it_sinfo->second;
		num_props_loaded = load_or_initialize(
			datastore, object, sinfo, prop_types
		);
		m_num_props_loaded += num_props_loaded;
		m_num_objects_loaded += static_cast<unsigned>(0 < num_props_loaded);
	}
	return commit_with(
		0 == m_num_objects_loaded
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
	Hord::Object::ID const object_id,
	IO::PropTypeBit prop_types
) noexcept try {
	auto& datastore = this->datastore();
	auto const& si_map = make_const(datastore).storage_info();

	auto* const object = datastore.find_ptr(object_id);
	if (!object) {
		return commit_error("object does not exist");
	}
	auto const it_sinfo = si_map.find(object->id());
	DUCT_ASSERTE(it_sinfo != si_map.cend());
	auto const& sinfo = it_sinfo->second;
	unsigned const num_props_loaded = load_or_initialize(
		datastore, *object, sinfo, prop_types
	);
	m_num_props_loaded += num_props_loaded;
	m_num_objects_loaded += static_cast<unsigned>(0 < num_props_loaded);
	return commit_with(
		0 == m_num_objects_loaded
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
