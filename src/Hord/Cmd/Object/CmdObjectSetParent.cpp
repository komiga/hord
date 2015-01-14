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
	m_id = object.get_id();

	// TODO: Check for collision in parent children
	auto& datastore = get_datastore();
	if (object.get_id() == new_parent) {
		return commit_error("object cannot be a child of itself");
	} else if (object.get_parent() == new_parent) {
		// NB: Circumvents Object::set_parent() making the parent ID_NULL
		return commit_with(Cmd::Result::success_no_action);
	}
	bool const success = Hord::Object::set_parent(object, datastore, new_parent);
	auto const end = datastore.get_root_objects().end();
	auto const it = datastore.get_root_objects().find(object.get_id());
	if (object.get_parent() == Hord::Object::ID_NULL) {
		if (it == end) {
			datastore.get_root_objects().emplace(object.get_id());
		}
	} else if (it != end) {
		datastore.get_root_objects().erase(it);
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
