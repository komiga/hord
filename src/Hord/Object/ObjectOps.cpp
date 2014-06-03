
#include <Hord/IO/Defs.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Object/Ops.hpp>

#include <ceformat/Format.hpp>
#include <ceformat/print.hpp>

#include <cassert>
#include <iostream>

namespace Hord {
namespace Object {

static bool
emplace_child(
	Object::Unit& object,
	Object::Unit& parent
) noexcept {
	assert(!parent.is_null());

	if (object.has_child(parent.get_id())) {
		return false;
	}

	// NB: children is a set, so no duplicates occur
	parent.get_children().emplace(object.get_id());
	if (object.get_parent() != parent.get_id()) {
		object.set_parent(parent.get_id());
		object.get_prop_states().assign(
			IO::PropType::identity,
			IO::PropState::modified
		);
	}
	return true;
}

bool
set_parent(
	Object::Unit& object,
	Hive::Unit& hive,
	Object::ID const new_parent
) noexcept {
	assert(!object.is_null());
	assert(Object::BaseType::Hive != object.get_base_type());

	if (new_parent == object.get_id()) {
		return false;
	}

	auto* const old_ptr = hive.find_ptr(object.get_parent());
	if (old_ptr) {
		if (object.get_parent() == new_parent) {
			// To ensure object is in parent's children collection
			return emplace_child(object, *old_ptr);
		} else {
			old_ptr->get_children().erase(object.get_id());
			object.set_parent(Object::ID_NULL);
		}
	}
	auto* const new_ptr = hive.find_ptr(new_parent);
	if (new_ptr) {
		return emplace_child(object, *new_ptr);
	} else {
		// If old_ptr and/or new_ptr are null or do not exist
		if (!object.get_parent().is_null()) {
			object.set_parent(Object::ID_NULL);
			object.get_prop_states().assign(
				IO::PropType::identity,
				IO::PropState::modified
			);
		}
		return false;
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
		printer.id.value()
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
		object.get_id_bare().value(),
		Object::get_base_type_name(type_info.type.base()),
		type_info.unit_name,
		object.get_slug()
	);
	return stream;
}

} // namespace Object
} // namespace Hord
