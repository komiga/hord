/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/aux.hpp>
#include <Hord/String.hpp>
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
	Data::Table::Iterator& it
) {
	it.remove();
	object.prop_states().assign(
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
	m_object_id = object.id();
	m_field_index = signed_cast(index);

	if (object.metadata().num_fields() <= index) {
		return commit_error("field index is out-of-bounds");
	}
	auto it = object.metadata().table().iterator_at(index);
	remove_field(object, it);
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
	String const& name
) noexcept try {
	m_object_id = object.id();
	m_field_index = -1;

	auto it = object.metadata().table().begin();
	for (; it.can_advance(); ++it) {
		auto const value_ref = it.get_field(Data::Metadata::COL_NAME);
		if (string_equal(name, value_ref.size, value_ref.data.string)) {
			m_field_index = signed_cast(it.index);
			remove_field(object, it);
			return commit();
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
