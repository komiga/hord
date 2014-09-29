
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

#define HORD_SCOPE_FUNC set_value
static void
set_value(
	Hord::Object::Unit& object,
	Hord::Data::FieldValue& value,
	Hord::Data::FieldValue const& new_value
) {
	if (new_value.type != value.type) {
		value = new_value;
	} else {
		switch (new_value.type) {
		case Hord::Data::FieldType::Text:
			if (new_value.str == value.str) {
				return;
			}
			value.str = new_value.str;
			break;

		case Hord::Data::FieldType::Number:
			if (new_value.num == value.num) {
				return;
			}
			value.num = new_value.num;
			break;

		case Hord::Data::FieldType::Boolean:
			if (new_value.bin == value.bin) {
				return;
			}
			value.bin = new_value.bin;
			break;
		}
	}
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
	Hord::Data::FieldValue const& new_value
) noexcept try {
	m_created = false;
	auto& fields = object.get_metadata().fields;
	if (fields.size() <= index) {
		return commit_error("field index is out-of-bounds");
	}
	set_value(object, fields[index].value, new_value);
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
	Hord::Data::FieldValue const& new_value,
	bool const create
) noexcept try {
	m_created = false;
	if (name.empty()) {
		return commit_error("empty name");
	}
	auto& fields = object.get_metadata().fields;
	for (auto it = fields.begin(); fields.end() != it; ++it) {
		if (name == it->name) {
			set_value(object, it->value, new_value);
			return commit();
		}
	}
	if (create) {
		fields.push_back(Hord::Data::MetaField{name, new_value});
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
