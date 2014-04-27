
#include <Hord/utility.hpp>
#include <Hord/String.hpp>
#include <Hord/Error.hpp>
#include <Hord/Log.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Object/Ops.hpp>
#include <Hord/Rule/Unit.hpp>
#include <Hord/Node/Unit.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/Prop.hpp>
#include <Hord/IO/PropTypeIterator.hpp>
#include <Hord/IO/PropStream.hpp>
#include <Hord/IO/Ops.hpp>

#include <type_traits>
#include <utility>

namespace Hord {
namespace IO {

namespace {

static void
load_prop_impl(
	Object::Unit& object,
	IO::InputPropStream& prop_stream
) {
	try {
		prop_stream.acquire();
		object.deserialize(prop_stream);
		prop_stream.release();
	} catch (...) {
		prop_stream.release(); // will not double-throw
		throw;
	}
}

static void
store_prop_impl(
	Object::Unit& object,
	IO::OutputPropStream& prop_stream
) {
	try {
		prop_stream.acquire();
		object.serialize(prop_stream);
		prop_stream.release();
	} catch (...) {
		prop_stream.release();
		throw;
	}
}

} // anonymous namespace

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
		load_prop_impl(object, prop_stream);
		return true;
	}
	return false;
}

bool
load_props(
	IO::Datastore& datastore,
	Object::Unit& object,
	IO::PropTypeBit const prop_types,
	bool const force
) {
	bool loaded_any = false;
	for (auto const prop_type : prop_types) {
		if (load_prop(datastore, object, prop_type, force)) {
			loaded_any = true;
		}
	}
	return loaded_any;
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
		store_prop_impl(object, prop_stream);
		return true;
	}
	return false;
}

bool
store_props(
	IO::Datastore& datastore,
	Object::Unit& object,
	IO::PropTypeBit const prop_types,
	bool const force
) {
	bool loaded_any = false;
	for (auto const prop_type : prop_types) {
		if (store_prop(datastore, object, prop_type, force)) {
			loaded_any = true;
		}
	}
	return loaded_any;
}

void
identify_all(
	IO::Datastore& datastore,
	Hive::Unit& hive
) {
	auto& states = hive.get_prop_states();
	if (states.all_data_initialized()) {
		return;
	}

	IO::load_prop(datastore, hive, IO::PropType::identity);
	IO::load_prop(datastore, hive, IO::PropType::primary);

	// Elide NULL_ID if it's in the idset
	{
		auto& idset = hive.get_idset();
		auto it = idset.find(Object::NULL_ID);
		if (idset.end() != it) {
			Log::acquire(Log::error)
				<< "null ID encountered in hive idset; erasing\n"
			;
			idset.erase(it);
		}
	}

	// Create and identify all objects
	auto const& idset = make_const(hive).get_idset();
	auto& objects = hive.get_objects();
	for (auto const object_id : idset) {
		// NB: Currently only nodes are hive children
		// TODO: Handle other object types from Driver
		auto& obj = *objects.emplace(
			object_id,
			Object::UPtr{new Node::Unit(object_id, Object::NULL_ID)}
		).first->second;

		IO::load_prop(datastore, obj, IO::PropType::identity);

		if (obj.get_id() == obj.get_parent()) {
			Log::acquire(Log::error)
				<< "Object " << obj << " has itself as parent\n"
			;
			obj.set_parent(Object::NULL_ID);
		} else if (
			Object::NULL_ID != obj.get_parent() &&
			idset.cend() == idset.find(obj.get_parent())
		) {
			Log::acquire(Log::error)
				<< "Object " << obj << " points to invalid parent "
				<< Object::IDPrinter{obj.get_parent()}
				<< "\n"
			;
			obj.set_parent(Object::NULL_ID);
		}
	}

	// Set parents after creating and identifying
	auto it = objects.end();
	for (auto& obj_pair : objects) {
		auto& obj = *obj_pair.second;
		if (Object::NULL_ID == obj.get_parent()) {
			Object::set_parent(obj, hive);
		} else if (
			objects.end() != (it = objects.find(obj.get_parent()))
		) {
			Object::set_parent(obj, *it->second);
		}
	}
}

} // namespace IO
} // namespace Hord
