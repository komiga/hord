
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

#define HORD_SCOPE_FUNC set_name
void
set_name(
	Object::Unit& object,
	String& name,
	String const& new_name
) {
	name = new_name;
	object.get_prop_states().assign(
		IO::PropType::metadata,
		IO::PropState::modified
	);
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC check_name // pseudo
bool
HORD_SCOPE_CLASS::check_name(
	Object::Unit const& object,
	String const& name
) {
	if (name.empty()) {
		return commit("new name must not be empty");
	}
	auto const& fields = object.get_metadata().fields;
	for (auto const& field : fields) {
		if (name == field.name) {
			return commit("another field already has this name");
		}
	}
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
	} else if (!check_name(object, new_name)) {
		return ok();
	}
	set_name(object, fields[index].name, new_name);
	return commit();
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
	if (!check_name(object, new_name)) {
		return ok();
	} else if (new_name == old_name) {
		return commit();
	}
	auto& fields = object.get_metadata().fields;
	for (auto it = fields.begin(); fields.end() != it; ++it) {
		if (old_name == it->name) {
			set_name(object, it->name, new_name);
			return commit();
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
