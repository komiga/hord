
#include <Hord/Object/Defs.hpp>
#include <Hord/Object/Ops.hpp>

#include <ceformat/print.hpp>

#include <cassert>
#include <iostream>

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

namespace {
static constexpr ceformat::Format const
	s_fmt_object_id{"%08x"},
	s_fmt_object_identity{"%08x$%s@%s"}
;
} // anonymous namespace

std::ostream&
operator<<(
	std::ostream& stream,
	Object::IDPrinter const& printer
) {
	ceformat::write<s_fmt_object_id>(
		stream,
		printer.id
	);
	return stream;
}

std::ostream&
operator<<(
	std::ostream& stream,
	Object::Unit const& object
) {
	ceformat::write<s_fmt_object_identity>(
		stream,
		object.get_id(),
		Object::get_type_name(object.get_type()),
		object.get_slug()
	);
	return stream;
}

} // namespace Object
} // namespace Hord
