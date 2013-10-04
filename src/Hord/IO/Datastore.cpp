
#include <Hord/Error.hpp>
#include <Hord/IO/Datastore.hpp>

#include <utility>

namespace Hord {
namespace IO {

// class Datastore implementation

#define HORD_SCOPE_CLASS_IDENT__ IO::Datastore

Datastore::Datastore(
	String root_path
) noexcept
	: m_root_path(std::move(root_path))
{}

Datastore::~Datastore() = default;

#define HORD_CLOSED_CHECK__								\
	if (!is_open()) {									\
		HORD_THROW_ERROR_SCOPED_FQN(					\
			ErrorCode::datastore_closed,				\
			"cannot perform this operation while"		\
			" datastore is closed"						\
		);												\
	}

#define HORD_LOCKED_CHECK__								\
	if (is_locked()) {									\
		HORD_THROW_ERROR_SCOPED_FQN(					\
			ErrorCode::datastore_locked,				\
			"cannot perform this operation while"		\
			" datastore is locked"						\
		);												\
	}

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
	HORD_LOCKED_CHECK__;
	if (is_open()) {
		close_impl();
	}
}
#undef HORD_SCOPE_FUNC_IDENT__

// acquire

#define HORD_SCOPE_FUNC_IDENT__ acquire_input_stream
std::istream&
Datastore::acquire_input_stream(
	IO::PropInfo const& prop_info
) {
	HORD_CLOSED_CHECK__;
	HORD_LOCKED_CHECK__;
	return acquire_input_stream_impl(prop_info);
}
#undef HORD_SCOPE_FUNC_IDENT__

#define HORD_SCOPE_FUNC_IDENT__ acquire_output_stream
std::ostream&
Datastore::acquire_output_stream(
	IO::PropInfo const& prop_info
) {
	HORD_CLOSED_CHECK__;
	HORD_LOCKED_CHECK__;
	return acquire_output_stream_impl(prop_info);
}
#undef HORD_SCOPE_FUNC_IDENT__

// release

#define HORD_RELEASE_CHECK__							\
	if (!is_locked()) {									\
		HORD_THROW_ERROR_SCOPED_FQN(					\
			ErrorCode::datastore_prop_not_locked,		\
			"prop is not locked"						\
		);												\
	}

#define HORD_SCOPE_FUNC_IDENT__ release_input_stream
void
Datastore::release_input_stream(
	IO::PropInfo const& prop_info
) {
	HORD_CLOSED_CHECK__;
	HORD_RELEASE_CHECK__;
	release_input_stream_impl(prop_info);
}
#undef HORD_SCOPE_FUNC_IDENT__

#define HORD_SCOPE_FUNC_IDENT__ release_output_stream
void
Datastore::release_output_stream(
	IO::PropInfo const& prop_info
) {
	HORD_CLOSED_CHECK__;
	HORD_RELEASE_CHECK__;
	release_output_stream_impl(prop_info);
}
#undef HORD_SCOPE_FUNC_IDENT__

#undef HORD_RELEASE_CHECK__

// objects

#define HORD_SCOPE_FUNC_IDENT__ generate_id
Object::ID
Datastore::generate_id(
	System::IDGenerator& generator
) const {
	HORD_CLOSED_CHECK__;
	return generate_id_impl(generator);
}
#undef HORD_SCOPE_FUNC_IDENT__

#define HORD_SCOPE_FUNC_IDENT__ create_object
void
Datastore::create_object(
	Object::ID const object_id,
	Object::Type const object_type
) {
	if (Object::Type::Node != object_type) {
		HORD_THROW_ERROR_SCOPED_FQN(
			ErrorCode::datastore_object_type_prohibited,
			"cannot create object other than Node"
		);
	}

	HORD_CLOSED_CHECK__;
	HORD_LOCKED_CHECK__;
	create_object_impl(object_id, object_type);
}
#undef HORD_SCOPE_FUNC_IDENT__

#define HORD_SCOPE_FUNC_IDENT__ destroy_object
void
Datastore::destroy_object(
	Object::ID const object_id
) {
	HORD_CLOSED_CHECK__;
	HORD_LOCKED_CHECK__;
	destroy_object_impl(object_id);
}
#undef HORD_SCOPE_FUNC_IDENT__

#undef HORD_CLOSED_CHECK__
#undef HORD_LOCKED_CHECK__

#undef HORD_SCOPE_CLASS_IDENT__

} // namespace IO
} // namespace Hord
