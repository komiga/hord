/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/aux.hpp>
#include <Hord/utility.hpp>
#include <Hord/Log.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Object/Unit.hpp>
#include <Hord/Cmd/Object.hpp>

#include <exception>

#include <Hord/detail/gr_core.hpp>

namespace Hord {
namespace Cmd {
namespace Object {

#define HORD_SCOPE_CLASS Cmd::Object::SetSlug

#define HORD_SCOPE_FUNC exec // pseudo
HORD_SCOPE_CLASS::exec_result_type
HORD_SCOPE_CLASS::operator()(
	Hord::Object::Unit& object,
	String new_slug
) noexcept try {
	m_id = object.get_id();

	// TODO: Check for collision in parent children
	if (new_slug.empty()) {
		return commit_error("slug cannot be empty");
	} else if (Hord::Object::SLUG_MAX_SIZE < new_slug.size()) {
		return commit_error("slug must be <= 255 code units");
	} else if (object.get_slug() == new_slug) {
		return commit_with(Cmd::Result::success_no_action);
	}
	object.set_slug(new_slug);
	object.get_prop_states().assign(
		IO::PropType::identity,
		IO::PropState::modified
	);
	return commit();
} catch (...) {
	notify_exception_current();
	return commit_error("unknown error");
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS

} // namespace Object
} // namespace Cmd
} // namespace Hord
