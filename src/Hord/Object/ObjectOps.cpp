
#include <Hord/IO/Defs.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Object/Ops.hpp>

#include <ceformat/Format.hpp>
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
		// NB: children is a set, so no duplicates occur
		new_parent.get_children().emplace(object.get_id());
		if (object.get_parent() != new_parent.get_id()) {
			object.set_parent(new_parent.get_id());
			object.get_prop_states().assign(
				IO::PropType::identity,
				IO::PropState::modified
			);
		}
		return true;
	}
}

namespace {
static constexpr ceformat::Format const
	s_fmt_object_id{"%08x"},
	s_fmt_object_identity{"%08x$%s:%s@%s"}
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
	auto const& type_info = object.get_type_info();
	ceformat::write<s_fmt_object_identity>(
		stream,
		object.get_id_bare(),
		Object::get_base_type_name(type_info.type.base()),
		type_info.unit_name,
		object.get_slug()
	);
	return stream;
}

} // namespace Object
} // namespace Hord
