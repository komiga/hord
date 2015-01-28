/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/aux.hpp>
#include <Hord/utility.hpp>
#include <Hord/Log.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Object/Unit.hpp>
#include <Hord/Object/Ops.hpp>
#include <Hord/Cmd/Object.hpp>

#include <exception>

#include <Hord/detail/gr_core.hpp>

namespace Hord {
namespace Cmd {
namespace Object {

#define HORD_SCOPE_CLASS Cmd::Object::SetParent

#define HORD_SCOPE_FUNC exec // pseudo
HORD_SCOPE_CLASS::exec_result_type
HORD_SCOPE_CLASS::operator()(
	Hord::Object::Unit& object,
	Hord::Object::ID const new_parent
) noexcept try {
	auto& datastore = this->datastore();

	m_object_id = object.id();
	auto const* parent_obj = datastore.find_ptr(new_parent);
	if (object.id() == new_parent) {
		return commit_error("cannot parent object to itself");
	} else if (object.parent() == new_parent) {
		// NB: Circumvents Object::set_parent() making the parent ID_NULL
		return commit_with(Cmd::Result::success_no_action);
	} else if (!new_parent.is_null() && !parent_obj) {
		return commit_error("new parent does not exist");
	} else if (parent_obj) {
		auto const& parent_children = parent_obj ? parent_obj->children() : datastore.root_objects();
		for (auto child_id : parent_children) {
			auto child_obj = datastore.find_ptr(child_id);
			if (child_obj && object.slug() == child_obj->slug()) {
				return commit_error("slug would not be unique in new parent");
			}
		}
	}

	bool const success = Hord::Object::set_parent(object, datastore, new_parent);
	auto const end = datastore.root_objects().end();
	auto const it = datastore.root_objects().find(object.id());
	if (object.parent().is_null()) {
		if (it == end) {
			datastore.root_objects().emplace(object.id());
		}
	} else if (it != end) {
		datastore.root_objects().erase(it);
	}
	if (success) {
		return commit();
	} else {
		return commit_error("Object::set_parent() failed");
	}
} catch (...) {
	notify_exception_current();
	return commit_error("unknown error");
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS

} // namespace Object
} // namespace Cmd
} // namespace Hord
