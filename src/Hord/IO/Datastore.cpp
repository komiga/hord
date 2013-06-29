
#include <Hord/Error.hpp>
#include <Hord/IO/Datastore.hpp>

#include <utility>
#include <cassert>

// TODO: Base exceptions [???]

namespace Hord {
namespace IO {

// class Datastore implementation

#define HORD_SCOPE_CLASS_IDENT__ IO::Datastore

Datastore::Datastore(
	String root_path,
	Hive::ID const id
) noexcept
	: m_root_path(std::move(root_path))
	, m_hive(id)
{}

Datastore::~Datastore() = default;

#define HORD_STATE_ASSERT_VALID__(x__)	\
	assert(								\
		State::RESERVED_FIRST > x__ &&	\
		State::RESERVED_LAST  < x__		\
	);

void
Datastore::enable_state(
	State const state
) noexcept {
	HORD_STATE_ASSERT_VALID__(state);
	m_states |= static_cast<unsigned>(state);
}

void
Datastore::disable_state(
	State const state
) noexcept {
	HORD_STATE_ASSERT_VALID__(state);
	m_states &= ~static_cast<unsigned>(state);
}

bool
Datastore::has_state(
	State const state
) const noexcept {
	HORD_STATE_ASSERT_VALID__(state);
	return m_states & static_cast<unsigned>(state);
}
#undef HORD_STATE_ASSERT_VALID__

#define HORD_SCOPE_FUNC_IDENT__ set_root_path
void
Datastore::set_root_path(
	String root_path
) {
	if (is_open()) {
		HORD_THROW_ERROR_SCOPED_FQN(
			ErrorCode::datastore_property_immutable,
			"cannot change root path while datastore is open"
		);
	}
	m_root_path.assign(std::move(root_path));
}
#undef HORD_SCOPE_FUNC_IDENT__

#define HORD_SCOPE_FUNC_IDENT__ open
void
Datastore::open() {
	if (is_open()) {
		HORD_THROW_ERROR_SCOPED_FQN(
			ErrorCode::datastore_open_already,
			"datastore is already open"
		);
	}
	open_impl();
}
#undef HORD_SCOPE_FUNC_IDENT__

#define HORD_SCOPE_FUNC_IDENT__ close
void
Datastore::close() {
	if (is_locked()) {
		HORD_THROW_ERROR_SCOPED_FQN(
			ErrorCode::datastore_locked,
			"cannot close datastore while locked"
		);
	} else if (is_open()) {
		close_impl();
	}
}
#undef HORD_SCOPE_FUNC_IDENT__

#define HORD_ACQUIRE_CHECK__									\
	if (!is_open()) {											\
		HORD_THROW_ERROR_SCOPED_FQN(							\
			ErrorCode::datastore_closed,						\
			"cannot acquire stream while datastore is closed"	\
		);														\
	} else if (is_locked()) {									\
		HORD_THROW_ERROR_SCOPED_FQN(							\
			ErrorCode::datastore_locked,						\
			"cannot acquire stream while datastore is locked"	\
		);														\
	}

#define HORD_SCOPE_FUNC_IDENT__ acquire_input_stream
std::istream&
Datastore::acquire_input_stream(
	IO::PropInfo const& prop_info
) {
	HORD_ACQUIRE_CHECK__;
	return acquire_input_stream_impl(prop_info);
}
#undef HORD_SCOPE_FUNC_IDENT__

#define HORD_SCOPE_FUNC_IDENT__ acquire_output_stream
std::ostream&
Datastore::acquire_output_stream(
	IO::PropInfo const& prop_info
) {
	HORD_ACQUIRE_CHECK__;
	return acquire_output_stream_impl(prop_info);
}
#undef HORD_SCOPE_FUNC_IDENT__
#undef HORD_ACQUIRE_CHECK__

#define HORD_RELEASE_CHECK__									\
	if (!is_open()) {											\
		HORD_THROW_ERROR_SCOPED_FQN(							\
			ErrorCode::datastore_closed,						\
			"cannot release stream while datastore is closed"	\
		);														\
	}

#define HORD_SCOPE_FUNC_IDENT__ release_input_stream
void
Datastore::release_input_stream(
	IO::PropInfo const& prop_info
) {
	HORD_RELEASE_CHECK__;
	release_input_stream_impl(prop_info);
}
#undef HORD_SCOPE_FUNC_IDENT__

#define HORD_SCOPE_FUNC_IDENT__ release_output_stream
void
Datastore::release_output_stream(
	IO::PropInfo const& prop_info
) {
	HORD_RELEASE_CHECK__;
	release_output_stream_impl(prop_info);
}
#undef HORD_SCOPE_FUNC_IDENT__
#undef HORD_RELEASE_CHECK__

#undef HORD_SCOPE_CLASS_IDENT__

} // namespace IO
} // namespace Hord
