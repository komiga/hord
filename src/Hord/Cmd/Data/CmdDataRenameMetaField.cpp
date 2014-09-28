
#include <Hord/aux.hpp>
#include <Hord/utility.hpp>
#include <Hord/Log.hpp>
#include <Hord/Data/Defs.hpp>
#include <Hord/Data/Metadata.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Cmd/Data.hpp>

#include <exception>

#include <Hord/detail/gr_core.hpp>

namespace Hord {
namespace Cmd {
namespace Data {

#define HORD_SCOPE_CLASS Cmd::Data::RenameMetaField

#define HORD_SCOPE_FUNC set_name // pseudo
HORD_SCOPE_CLASS::result_type
HORD_SCOPE_CLASS::set_name(
	Object::Unit& object,
	Hord::Data::MetaField& field,
	String const& new_name
) {
	if (new_name == field.name) {
		return true;
	} else if (new_name.empty()) {
		set_message("new name must not be empty");
		return false;
	}
	auto const& fields = object.get_metadata().fields;
	for (auto const& it_field : fields) {
		if (&it_field != &field && new_name == it_field.name) {
			set_message("another field already has this name");
			return false;
		}
	}
	field.name = new_name;
	object.get_prop_states().assign(
		IO::PropType::metadata,
		IO::PropState::modified
	);
	return true;
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC exec // pseudo
HORD_SCOPE_CLASS::result_type
HORD_SCOPE_CLASS::operator()(
	Object::Unit& object,
	unsigned const index,
	String const& new_name
) noexcept try {
	auto& fields = object.get_metadata().fields;
	if (fields.size() <= index) {
		return commit("field index is out-of-bounds");
	}
	return commit_with(
		set_name(object, fields[index], new_name)
	);
} catch (...) {
	Log::acquire(Log::error)
		<< DUCT_GR_MSG_FQN("error initializing:\n")
	;
	Log::report_error_ptr(std::current_exception());
	return commit("unknown error");
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC exec // pseudo
HORD_SCOPE_CLASS::result_type
HORD_SCOPE_CLASS::operator()(
	Object::Unit& object,
	String const& old_name,
	String const& new_name
) noexcept try {
	auto& fields = object.get_metadata().fields;
	for (auto& field : fields) {
		if (old_name == field.name) {
			return commit_with(
				set_name(object, field, new_name)
			);
		}
	}
	return commit("field does not exist");
} catch (...) {
	Log::acquire(Log::error)
		<< DUCT_GR_MSG_FQN("error initializing:\n")
	;
	Log::report_error_ptr(std::current_exception());
	return commit("unknown error");
}
#undef HORD_SCOPE_FUNC

} // namespace Data
} // namespace Cmd
} // namespace Hord