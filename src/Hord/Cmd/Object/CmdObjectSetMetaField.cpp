
#include <Hord/aux.hpp>
#include <Hord/utility.hpp>
#include <Hord/Log.hpp>
#include <Hord/Data/Defs.hpp>
#include <Hord/Data/Metadata.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Cmd/Object.hpp>

#include <exception>

#include <Hord/detail/gr_core.hpp>

namespace Hord {
namespace Cmd {
namespace Object {

#define HORD_SCOPE_CLASS Cmd::Object::SetMetaField

#define HORD_SCOPE_FUNC set_value // pseudo
static void
set_value(
	Hord::Object::Unit& object,
	Hord::Data::Table::Iterator& it,
	Hord::Data::ValueRef const& new_value
) {
	it.set_field(Data::Metadata::COL_VALUE, new_value);
	object.get_prop_states().assign(
		IO::PropType::metadata,
		IO::PropState::modified
	);
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC exec // pseudo
HORD_SCOPE_CLASS::exec_result_type
HORD_SCOPE_CLASS::operator()(
	Hord::Object::Unit& object,
	unsigned const index,
	Hord::Data::ValueRef const& new_value
) noexcept try {
	m_created = false;
	if (object.get_metadata().num_fields() <= index) {
		return commit_error("field index is out-of-bounds");
	}
	auto it = object.get_metadata().table().iterator_at(index);
	set_value(object, it, new_value);
	return commit();
} catch (...) {
	notify_exception_current();
	return commit_error("unknown error");
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC exec // pseudo
HORD_SCOPE_CLASS::exec_result_type
HORD_SCOPE_CLASS::operator()(
	Hord::Object::Unit& object,
	String const& name,
	Hord::Data::ValueRef const& new_value,
	bool const create
) noexcept try {
	m_created = false;
	if (name.empty()) {
		return commit_error("empty name");
	}
	auto it = object.get_metadata().table().begin();
	for (; it.can_advance(); ++it) {
		auto const value_ref = it.get_field(Data::Metadata::COL_NAME);
		if (string_equal(name, value_ref.size, value_ref.data.string)) {
			set_value(object, it, new_value);
			return commit();
		}
	}
	if (create) {
		Data::ValueRef fields[2];
		fields[Data::Metadata::COL_NAME] = {name};
		fields[Data::Metadata::COL_VALUE] = {new_value};
		object.get_metadata().table().push_back(2, fields);
		object.get_prop_states().assign(
			IO::PropType::metadata,
			IO::PropState::modified
		);
		m_created = true;
		return commit();
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
