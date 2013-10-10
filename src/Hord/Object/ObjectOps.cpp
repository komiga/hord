
#include <Hord/Object/Defs.hpp>
#include <Hord/Object/Ops.hpp>

#include <cassert>

namespace Hord {
namespace Object {

void
unset_parent(
	Object::Unit& object,
	Object::Unit& parent
) noexcept {
	assert(!object.is_null());

	parent.get_children().erase(object.get_id());
	object.set_parent(Object::NULL_ID);
}

bool
set_parent(
	Object::Unit& object,
	Object::Unit& new_parent
) noexcept {
	assert(!object.is_null());

	if (object.get_id() == new_parent.get_id()) {
		return false;
	} else {
		new_parent.get_children().emplace(object.get_id());
		object.set_parent(
			Object::Type::Hive == new_parent.get_type()
			? Object::NULL_ID
			: new_parent.get_id()
		);
		return true;
	}
}

} // namespace Object
} // namespace Hord
