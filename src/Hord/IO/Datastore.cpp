/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/utility.hpp>
#include <Hord/IO/Datastore.hpp>

#include <utility>

#include <Hord/detail/gr_core.hpp>

namespace Hord {
namespace IO {

// class Datastore implementation

#define HORD_SCOPE_CLASS IO::Datastore

Datastore::Datastore(
	IO::Datastore::type_info const& tinfo,
	String root_path
) noexcept
	: m_type_info(&tinfo)
	, m_id(hash_string(root_path))
	, m_states()
	, m_root_path(std::move(root_path))
	, m_storage_info()
	, m_objects()
	, m_root_objects()
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

#define HORD_SCOPE_FUNC open
void
Datastore::open(
	bool const create_if_nonexistent
) {
	if (is_open()) {
		HORD_THROW_FQN(
			ErrorCode::datastore_open_already,
			"datastore is already open"
		);
	}
	try {
		open_impl(create_if_nonexistent);
	} catch (...) {
		disable_state(State::opened);
		throw;
	}
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC close
void
Datastore::close() {
	HORD_LOCKED_CHECK_;
	if (is_open()) {
		close_impl();
		disable_state(State::initialized);
		m_root_objects.clear();
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

#define HORD_RELEASE_CHECK_								\
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
Datastore::storage_info_map_type::const_iterator
Datastore::create_object(
	Object::ID const object_id,
	Object::type_info const& type_info,
	IO::Linkage const linkage
) {
	HORD_CLOSED_CHECK_;
	HORD_LOCKED_CHECK_;
	if (m_storage_info.cend() != m_storage_info.find(object_id)) {
		HORD_THROW_FQN(
			ErrorCode::datastore_object_already_exists,
			"object already exists"
		);
	}

	return create_object_impl(object_id, type_info, linkage);
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

static Object::Unit const*
find_by_slug(
	IO::Datastore const& datastore,
	IO::Datastore::id_set_type const& children,
	unsigned const slug_size,
	char const* slug
) {
	for (auto const id : children) {
		auto const* object = datastore.find_ptr(id);
		if (object && string_equal(object->get_slug(), slug_size, slug)) {
			return object;
		}
	}
	return nullptr;
}

Object::Unit const*
Datastore::find_ptr_path(
	String const& path
) const noexcept {
	if (path.empty() || (path.size() == 1 && path[0] == '/')) {
		return nullptr;
	}
	Object::Unit const* object = nullptr;
	auto a = path.find('/');
	decltype(a) b;
	while (a != String::npos) {
		b = path.find(++a, '/');
		object = find_by_slug(
			*this,
			object ? object->get_children() : m_root_objects,
			min_ce(b, path.size()) - a, &path[a]
		);
		if (!object || b == String::npos) {
			break;
		}
		a = path.find(b + 1, '/');
	}
	return object;
}

#undef HORD_CLOSED_CHECK_
#undef HORD_LOCKED_CHECK_

#undef HORD_SCOPE_CLASS

} // namespace IO
} // namespace Hord
