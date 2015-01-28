/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/aux.hpp>
#include <Hord/String.hpp>
#include <Hord/utility.hpp>
#include <Hord/Log.hpp>
#include <Hord/Data/Defs.hpp>
#include <Hord/Data/Table.hpp>
#include <Hord/Data/Metadata.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Cmd/Object.hpp>

#include <exception>

#include <Hord/detail/gr_core.hpp>

namespace Hord {
namespace Cmd {
namespace Object {

#define HORD_SCOPE_CLASS Cmd::Object::RenameMetaField

#define HORD_SCOPE_FUNC set_name // pseudo
Cmd::Result
HORD_SCOPE_CLASS::set_name(
	Hord::Object::Unit& object,
	Hord::Data::Table::Iterator& it,
	String const& new_name
) {
	if (new_name.empty()) {
		set_message("new name must not be empty");
		return Cmd::Result::error;
	}
	auto it_search = object.metadata().table().begin();
	for (; it_search.can_advance(); ++it_search) {
		auto const value_ref = it_search.get_field(Data::Metadata::COL_NAME);
		if (!string_equal(new_name, value_ref.size, value_ref.data.string)) {
			// Do nothing
		} else if (it_search == it) {
			return Cmd::Result::success_no_action;
		} else {
			set_message("another field already has this name");
			return Cmd::Result::error;
		}
	}

	it.set_field(Data::Metadata::COL_NAME, new_name);
	object.prop_states().assign(
		IO::PropType::metadata,
		IO::PropState::modified
	);
	return Cmd::Result::success;
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC exec // pseudo
HORD_SCOPE_CLASS::exec_result_type
HORD_SCOPE_CLASS::operator()(
	Hord::Object::Unit& object,
	unsigned const index,
	String const& new_name
) noexcept try {
	m_object_id = object.id();
	m_field_index = signed_cast(index);

	if (object.metadata().num_fields() <= index) {
		return commit_error("field index is out-of-bounds");
	}
	auto it = object.metadata().table().iterator_at(index);
	return commit_with(set_name(object, it, new_name));
} catch (...) {
	notify_exception_current();
	return commit_error("unknown error");
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC exec // pseudo
HORD_SCOPE_CLASS::exec_result_type
HORD_SCOPE_CLASS::operator()(
	Hord::Object::Unit& object,
	String const& old_name,
	String const& new_name
) noexcept try {
	m_object_id = object.id();
	m_field_index = -1;

	auto it = object.metadata().table().begin();
	for (; it.can_advance(); ++it) {
		auto const value_ref = it.get_field(Data::Metadata::COL_NAME);
		if (string_equal(old_name, value_ref.size, value_ref.data.string)) {
			m_field_index = signed_cast(it.index);
			return commit_with(set_name(object, it, new_name));
		}
	}
	return commit_error("field does not exist");
} catch (...) {
	notify_exception_current();
	return commit_error("unknown error");
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS

} // namespace Object
} // namespace Cmd
} // namespace Hord
