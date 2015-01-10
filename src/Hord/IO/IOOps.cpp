/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/utility.hpp>
#include <Hord/String.hpp>
#include <Hord/Error.hpp>
#include <Hord/Log.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Object/Ops.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/Prop.hpp>
#include <Hord/IO/PropTypeIterator.hpp>
#include <Hord/IO/PropStream.hpp>
#include <Hord/IO/Ops.hpp>

#include <type_traits>
#include <utility>

namespace Hord {
namespace IO {

bool
load_prop(
	IO::Datastore& datastore,
	Object::Unit& object,
	IO::PropType const prop_type,
	bool const force
) {
	if (force || !object.get_prop_states().is_initialized(prop_type)) {
		IO::InputPropStream prop_stream{
			datastore, {object, prop_type}
		};
		try {
			prop_stream.acquire();
			object.deserialize(prop_stream);
			prop_stream.release();
		} catch (...) {
			prop_stream.release(); // will not double-throw
			throw;
		}
		object.get_prop_states().assign(prop_type, IO::PropState::original);
		return true;
	}
	return false;
}

bool
load_prop_weak(
	IO::Datastore& datastore,
	Object::Unit& object,
	IO::PropType const prop_type,
	std::istream& stream,
	bool const force
) {
	if (force || !object.get_prop_states().is_initialized(prop_type)) {
		IO::InputPropStream prop_stream{
			datastore, {object, prop_type}
		};
		try {
			prop_stream.acquire_weak(stream);
			object.deserialize(prop_stream);
			prop_stream.release();
		} catch (...) {
			prop_stream.release(); // will not double-throw
			throw;
		}
		return true;
	}
	return false;
}

unsigned
load_props(
	IO::Datastore& datastore,
	Object::Unit& object,
	IO::PropTypeBit const prop_types,
	bool const force
) {
	unsigned num_stored = 0;
	for (auto const prop_type : prop_types) {
		if (load_prop(datastore, object, prop_type, force)) {
			++num_stored;
		}
	}
	return num_stored;
}

unsigned
load_props_weak(
	IO::Datastore& datastore,
	Object::Unit& object,
	IO::PropTypeBit const prop_types,
	std::istream& stream,
	bool const force
) {
	unsigned num_stored = 0;
	for (auto const prop_type : prop_types) {
		if (load_prop_weak(datastore, object, prop_type, stream, force)) {
			++num_stored;
		}
	}
	return num_stored;
}

bool
store_prop(
	IO::Datastore& datastore,
	Object::Unit& object,
	IO::PropType const prop_type,
	bool const force
) {
	if (
		force ||
		object.get_prop_states().has(prop_type, IO::PropState::modified)
	) {
		IO::OutputPropStream prop_stream{
			datastore, {object, prop_type}
		};
		try {
			prop_stream.acquire();
			object.serialize(prop_stream);
			prop_stream.release();
		} catch (...) {
			prop_stream.release();
			throw;
		}
		object.get_prop_states().assign(prop_type, IO::PropState::original);
		return true;
	}
	return false;
}

bool
store_prop_weak(
	IO::Datastore& datastore,
	Object::Unit& object,
	IO::PropType const prop_type,
	std::ostream& stream,
	bool const force
) {
	if (
		force ||
		object.get_prop_states().has(prop_type, IO::PropState::modified)
	) {
		IO::OutputPropStream prop_stream{
			datastore, {object, prop_type}
		};
		try {
			prop_stream.acquire_weak(stream);
			object.serialize(prop_stream);
			prop_stream.release();
		} catch (...) {
			prop_stream.release();
			throw;
		}
		return true;
	}
	return false;
}

unsigned
store_props(
	IO::Datastore& datastore,
	Object::Unit& object,
	IO::PropTypeBit const prop_types,
	bool const force
) {
	unsigned num_stored = 0u;
	for (auto const prop_type : prop_types) {
		if (store_prop(datastore, object, prop_type, force)) {
			++num_stored;
		}
	}
	return num_stored;
}

unsigned
store_props_weak(
	IO::Datastore& datastore,
	Object::Unit& object,
	IO::PropTypeBit const prop_types,
	std::ostream& stream,
	bool const force
) {
	unsigned num_stored = 0u;
	for (auto const prop_type : prop_types) {
		if (store_prop_weak(datastore, object, prop_type, stream, force)) {
			++num_stored;
		}
	}
	return num_stored;
}

} // namespace IO
} // namespace Hord
