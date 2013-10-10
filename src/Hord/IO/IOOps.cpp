
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
	IO::PropType const type
) {
	auto& states = object.get_prop_states(); 
	if (!states.is_initialized(type)) {
		return false;
	}

	IO::InputPropStream stream{
		datastore,
		{object.get_id(), type}
	};
	try {
		stream.acquire();
		object.deserialize(stream);
		stream.release();
	} catch (Error& e) {
		stream.release(); // will not double-throw
		throw;
	}

	states.assign(type, IO::PropState::original);

	return true;
}

bool
store_prop(
	IO::Datastore& datastore,
	Object::Unit& object,
	IO::PropType const type
) {
	auto& states = object.get_prop_states(); 
	if (!states.is_initialized(type)) {
		return false;
	}

	IO::OutputPropStream stream{
		datastore,
		{object.get_id(), type}
	};
	try {
		stream.acquire();
		object.serialize(stream);
		stream.release();
	} catch (...) {
		stream.release();
		throw;
	}

	states.assign(type, IO::PropState::original);

	return true;
}

void
identify_all(
	IO::Datastore& datastore,
	Hive::Unit& hive
) {
	IO::PropStateStore& states = hive.get_prop_states();

	if (states.all_data_initialized()) {
		return;
	}

	IO::load_prop(datastore, hive, IO::PropType::identity);
	IO::load_prop(datastore, hive, IO::PropType::primary);

	// Elide NULL_ID if it's in the idset
	{
		auto& idset = hive.get_idset();
		auto it = idset.find(Object::NULL_ID);
		if (idset.end() == it) {
			Log::acquire(Log::error)
				<< Log::Pre::current
				<< "null ID encountered in hive idset\n"
			;
			idset.erase(it);
		}
	}

	// Create and identify all objects
	auto const& idset = make_const(hive).get_idset();
	auto& objects = hive.get_objects();
	for (auto const object_id : idset) {
		// NB: Currently only nodes are hive children
		auto& obj = *objects.emplace(
			object_id,
			Object::UPtr{new Node::Unit(object_id, Object::NULL_ID)}
		).first->second;

		IO::load_prop(datastore, obj, IO::PropType::identity);

		if (obj.get_id() == obj.get_parent()) {
			obj.set_parent(Object::NULL_ID);
		} else if (
			Object::NULL_ID != obj.get_parent() &&
			idset.cend() == idset.find(obj.get_parent())
		) {
			Log::acquire(Log::error)
				<< Log::Pre::current
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
