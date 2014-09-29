
#include <Hord/utility.hpp>
#include <Hord/Log.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/Prop.hpp>
#include <Hord/IO/Datastore.hpp>
#include <Hord/Object/Ops.hpp>
#include <Hord/Hive/Defs.hpp>
#include <Hord/Hive/Unit.hpp>
#include <Hord/Node/Defs.hpp>
#include <Hord/Node/Unit.hpp>
#include <Hord/Cmd/Node.hpp>
#include <Hord/System/Driver.hpp>
#include <Hord/System/Context.hpp>

#include <exception>

#include <Hord/detail/gr_core.hpp>

namespace Hord {
namespace Cmd {
namespace Node {

#define HORD_SCOPE_CLASS Cmd::Node::Create

#define HORD_SCOPE_FUNC commit_impl
HORD_CMD_IMPL_COMMIT_DEF(HORD_SCOPE_CLASS) {
	if (bad()) {
		m_id = Hord::Node::ID_NULL;
	}
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC exec // pseudo
HORD_SCOPE_CLASS::exec_result_type
HORD_SCOPE_CLASS::operator()(
	Object::ID const parent,
	Hord::Node::ID const layout_ref,
	String const& slug,
	Hord::Node::UnitType const unit_type
) noexcept try {
	auto& driver = get_driver();
	auto& datastore = get_datastore();
	auto& hive = get_hive();

	// Validate
	if (
		Object::ID_NULL == parent || (
			Object::ID_HIVE != parent &&
			!hive.has_object(parent)
		)
	) {
		return commit("parent not found");
	} else if (
		Hord::Node::ID_NULL != layout_ref &&
		!hive.has_object(layout_ref)
	) {
		return commit("layout ref not found");
	} else if (
		Hord::Node::ID_NULL != layout_ref &&
		Object::BaseType::Node != hive.find_ptr(
			layout_ref
		)->get_base_type()
	) {
		return commit("layout ref not a node");
	} else if (slug.empty()) {
		return commit("slug empty");
	} else if (Object::SLUG_MAX_SIZE < slug.size()) {
		return commit("slug too long");
	} else {
		for (auto const& pair : hive.get_objects()) {
			auto const* const object = pair.second.get();
			if (
				nullptr      != object &&
				parent == object->get_parent() &&
				slug   == object->get_slug()
			) {
				return commit("slug already exists");
			}
		}
	}

	// Create
	auto const* const
	tinfo = driver.get_object_type_info(
		Hord::Node::Type{unit_type}
	);
	if (nullptr == tinfo) {
		return commit("unknown unit type");
	}
	// NB: Leak ErrorCode::datastore_closed
	m_id = Hord::Node::ID{datastore.generate_id(
	    driver.get_id_generator()
	)};
	auto obj = tinfo->construct(m_id, Object::ID_NULL);
	if (nullptr == obj) {
		return commit("allocation failed");
	}
	auto emplace_pair = hive.get_objects().emplace(m_id, std::move(obj));
	if (!emplace_pair.second) {
		return commit("id already exists");
	}
	datastore.create_object(
		m_id, *tinfo, IO::Linkage::resident
	);

	// Properties
	auto& node = static_cast<Hord::Node::Unit&>(
		*emplace_pair.first->second
	);
	Object::set_parent(node, hive, parent);
	node.set_slug(slug);
	node.set_layout_ref(layout_ref);
	node.get_prop_states().assign_all(IO::PropState::modified);

	return commit();
} catch (Error const& err) {
	notify_exception_current();
	if (ErrorCode::datastore_object_already_exists == err.get_code()) {
		return commit("id already exists");
	}
	return commit("unknown error");
} catch (...) {
	notify_exception_current();
	return commit("unknown error");
}
#undef HORD_SCOPE_FUNC

} // namespace Node
} // namespace Cmd
} // namespace Hord
