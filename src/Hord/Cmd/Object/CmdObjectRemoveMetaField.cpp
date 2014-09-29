
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

#define HORD_SCOPE_CLASS Cmd::Object::RemoveMetaField

#define HORD_SCOPE_FUNC remove_field
static void
remove_field(
	Hord::Object::Unit& object,
	Hord::Data::Metadata::field_vector_type::const_iterator const it
) {
	object.get_metadata().fields.erase(it);
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
	unsigned const index
) noexcept try {
	auto& fields = object.get_metadata().fields;
	if (fields.size() <= index) {
		return commit("field index is out-of-bounds");
	}
	remove_field(object, fields.cbegin() + index);
	return commit();
} catch (...) {
	notify_exception_current();
	return commit("unknown error");
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC exec // pseudo
HORD_SCOPE_CLASS::exec_result_type
HORD_SCOPE_CLASS::operator()(
	Hord::Object::Unit& object,
	String const& name
) noexcept try {
	auto const& fields = object.get_metadata().fields;
	for (auto it = fields.cbegin(); fields.cend() != it; ++it) {
		if (name == it->name) {
			remove_field(object, it);
			return commit();
		}
	}
	return commit("field does not exist");
} catch (...) {
	notify_exception_current();
	return commit("unknown error");
}
#undef HORD_SCOPE_FUNC

} // namespace Object
} // namespace Cmd
} // namespace Hord
