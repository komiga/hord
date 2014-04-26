/**
@file IO/Ops.hpp
@brief IO operations.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_IO_SERIALIZATION_HPP_
#define HORD_IO_SERIALIZATION_HPP_

#include <Hord/config.hpp>
#include <Hord/Object/Unit.hpp>
#include <Hord/Hive/Unit.hpp>
#include <Hord/IO/Datastore.hpp>

namespace Hord {
namespace IO {

// Forward declarations

/**
	@addtogroup io
	@{
*/

// TODO: exception specifications

/**
	Load a prop.

	@note Unless @a force is @c true, this will only load the prop
	if it is uninitialized.

	@throws Error{...}
	From Object::Unit::deserialize().

	@returns Whether the prop was loaded.
	@param datastore %Datastore.
	@param object Object to operate on.
	@param prop_type Prop to load.
	@param force Whether to force load, disregarding prop state.

	@sa Object::Unit::deserialize()
*/
bool
load_prop(
	IO::Datastore& datastore,
	Object::Unit& object,
	IO::PropType const prop_type,
	bool const force = false
);

/**
	Store a prop.

	@note Unless @a force is @c true, this will only store the prop
	if it is modified.

	@throws Error{...}
	From Object::Unit::serialize().

	@returns Whether the prop was stored.
	@param datastore %Datastore.
	@param object Object.
	@param prop_type Prop to store.
	@param force Whether to force store, disregarding prop state.

	@sa Object::Unit::serialize()
*/
bool
store_prop(
	IO::Datastore& datastore,
	Object::Unit& object,
	IO::PropType const prop_type,
	bool const force = false
);

/**
	Identify all objects in a data-uninitialized hive.

	@note Because this function is a part of the initialization
	sequence proper, it returns immediately if the hive's data
	prop has already been loaded. Identification should be properly
	maintained by the runtime, so this sort of operation should not
	be needed after initialization.

	@par
	@note This will handle degeneracies in object relationships.

	@param datastore %Datastore.
	@param hive Hive.

	@sa IO::StorageState::placeholder,
		IO::StorageState::placeholder_identified
*/
void
identify_all(
	IO::Datastore& datastore,
	Hive::Unit& hive
);

/** @} */ // end of doc-group io

} // namespace IO
} // namespace Hord

#endif // HORD_IO_SERIALIZATION_HPP_
