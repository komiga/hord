/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/utility.hpp>
#include <Hord/Log.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/Prop.hpp>
#include <Hord/IO/Datastore.hpp>
#include <Hord/Object/Ops.hpp>
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
	Hord::Object::ID const parent,
	Hord::Node::ID const layout_ref,
	String const& slug,
	Hord::Node::UnitType const unit_type
) noexcept try {
	auto& driver = get_driver();
	auto& datastore = get_datastore();

	// Validate
	if (
		parent != Hord::Object::ID_NULL &&
		!datastore.has_object(parent)
	) {
		return commit_error("parent not found");
	} else if (
		Hord::Node::ID_NULL != layout_ref &&
		!datastore.has_object(layout_ref)
	) {
		return commit_error("layout ref not found");
	} else if (
		Hord::Node::ID_NULL != layout_ref &&
		Hord::Object::BaseType::Node != datastore.find_ptr(
			layout_ref
		)->get_base_type()
	) {
		return commit_error("layout ref not a node");
	} else if (slug.empty()) {
		return commit_error("slug empty");
	} else if (Hord::Object::SLUG_MAX_SIZE < slug.size()) {
		return commit_error("slug too long");
	} else {
		// TODO: Only check children of parent
		// (all should be resident when command is executed)
		for (auto const& pair : datastore.get_objects()) {
			auto const* const object = pair.second.get();
			if (
				nullptr != object &&
				parent  == object->get_parent() &&
				slug    == object->get_slug()
			) {
				return commit_error("slug already exists");
			}
		}
	}

	// Create
	auto const* const
	tinfo = driver.get_object_type_info(
		Hord::Node::Type{unit_type}
	);
	if (nullptr == tinfo) {
		return commit_error("unknown unit type");
	}
	// NB: Leak ErrorCode::datastore_closed
	m_id = Hord::Node::ID{datastore.generate_id(
	    driver.get_id_generator()
	)};
	auto obj = tinfo->construct(m_id, Hord::Object::ID_NULL);
	if (nullptr == obj) {
		return commit_error("allocation failed");
	}
	auto emplace_pair = datastore.get_objects().emplace(m_id, std::move(obj));
	if (!emplace_pair.second) {
		return commit_error("id already exists");
	}
	datastore.create_object(
		m_id, *tinfo, IO::Linkage::resident
	);

	// Properties
	auto& node = static_cast<Hord::Node::Unit&>(
		*emplace_pair.first->second
	);
	Hord::Object::set_parent(node, datastore, parent);
	node.set_slug(slug);
	node.set_layout_ref(layout_ref);
	node.get_prop_states().assign_all(IO::PropState::modified);

	return commit();
} catch (Error const& err) {
	notify_exception_current();
	if (ErrorCode::datastore_object_already_exists == err.get_code()) {
		return commit_error("id already exists");
	}
	return commit_error("unknown error");
} catch (...) {
	notify_exception_current();
	return commit_error("unknown error");
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS

} // namespace Node
} // namespace Cmd
} // namespace Hord
