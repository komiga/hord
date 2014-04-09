
#include <Hord/IO/Datastore.hpp>

#include <utility>

#include <Hord/detail/gr_core.hpp>

namespace Hord {
namespace IO {

// class Datastore implementation

#define HORD_SCOPE_CLASS IO::Datastore

Datastore::Datastore(
	String root_path
) noexcept
	: m_root_path(std::move(root_path))
{}

Datastore::~Datastore() = default;

#define HORD_CLOSED_CHECK_								\
	if (!is_open()) {									\
		HORD_THROW_FQN(									\
			ErrorCode::datastore_closed,				\
			"cannot perform this operation while"		\
			" datastore is closed"						\
		);												\
	}

#define HORD_LOCKED_CHECK_								\
	if (is_locked()) {									\
		HORD_THROW_FQN(									\
			ErrorCode::datastore_locked,				\
			"cannot perform this operation while"		\
			" datastore is locked"						\
		);												\
	}

#define HORD_SCOPE_FUNC set_root_path
void
Datastore::set_root_path(
	String root_path
) {
	if (is_open()) {
		HORD_THROW_FQN(
			ErrorCode::datastore_property_immutable,
			"cannot change root path while datastore is open"
		);
	}
	m_root_path.assign(std::move(root_path));
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC open
void
Datastore::open() {
	if (is_open()) {
		HORD_THROW_FQN(
			ErrorCode::datastore_open_already,
			"datastore is already open"
		);
	}
	open_impl();
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC close
void
Datastore::close() {
	HORD_LOCKED_CHECK_;
	if (is_open()) {
		close_impl();
	}
}
#undef HORD_SCOPE_FUNC

// acquire

#define HORD_SCOPE_FUNC acquire_input_stream
std::istream&
Datastore::acquire_input_stream(
	IO::PropInfo const& prop_info
) {
	HORD_CLOSED_CHECK_;
	HORD_LOCKED_CHECK_;
	return acquire_input_stream_impl(prop_info);
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC acquire_output_stream
std::ostream&
Datastore::acquire_output_stream(
	IO::PropInfo const& prop_info
) {
	HORD_CLOSED_CHECK_;
	HORD_LOCKED_CHECK_;
	return acquire_output_stream_impl(prop_info);
}
#undef HORD_SCOPE_FUNC

// release

#define HORD_RELEASE_CHECK_							\
	if (!is_locked()) {									\
		HORD_THROW_FQN(									\
			ErrorCode::datastore_prop_not_locked,		\
			"prop is not locked"						\
		);												\
	}

#define HORD_SCOPE_FUNC release_input_stream
void
Datastore::release_input_stream(
	IO::PropInfo const& prop_info
) {
	HORD_CLOSED_CHECK_;
	HORD_RELEASE_CHECK_;
	release_input_stream_impl(prop_info);
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC release_output_stream
void
Datastore::release_output_stream(
	IO::PropInfo const& prop_info
) {
	HORD_CLOSED_CHECK_;
	HORD_RELEASE_CHECK_;
	release_output_stream_impl(prop_info);
}
#undef HORD_SCOPE_FUNC

#undef HORD_RELEASE_CHECK_

// objects

#define HORD_SCOPE_FUNC generate_id
Object::ID
Datastore::generate_id(
	System::IDGenerator& generator
) const {
	HORD_CLOSED_CHECK_;
	return generate_id_impl(generator);
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC create_object
void
Datastore::create_object(
	Object::ID const object_id,
	Object::Type const object_type
) {
	if (Object::Type::Node != object_type) {
		HORD_THROW_FQN(
			ErrorCode::datastore_object_type_prohibited,
			"cannot create object other than Node"
		);
	} else if (Object::NULL_ID == object_id) {
		HORD_THROW_FQN(
			ErrorCode::datastore_object_already_exists,
			"null object already exists (alias to Hive)"
		);
	}

	HORD_CLOSED_CHECK_;
	HORD_LOCKED_CHECK_;
	create_object_impl(object_id, object_type);
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC destroy_object
void
Datastore::destroy_object(
	Object::ID const object_id
) {
	HORD_CLOSED_CHECK_;
	HORD_LOCKED_CHECK_;
	destroy_object_impl(object_id);
}
#undef HORD_SCOPE_FUNC

#undef HORD_CLOSED_CHECK_
#undef HORD_LOCKED_CHECK_

#undef HORD_SCOPE_CLASS

} // namespace IO
} // namespace Hord
