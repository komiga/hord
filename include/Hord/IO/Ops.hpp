/**
@file IO/serialization.hpp
@brief Serialization.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
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
	Load a prop if it is uninitialized.

	@returns Whether the prop was loaded.
	@param datastore %Datastore.
	@param object Object to operate on.
	@param type Prop to load.

	@sa IO::PropState::uninitialized
*/
bool
load_prop(
	IO::Datastore& datastore,
	Object::Unit& object,
	IO::PropType const prop_type
);

/**
	Store a prop if it is modified.

	@returns Whether the prop was loaded.
	@param datastore %Datastore.
	@param object Object.
	@param type Prop to store.

	@sa IO::PropState::modified
*/
bool
store_prop(
	IO::Datastore& datastore,
	Object::Unit& object,
	IO::PropType const prop_type
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

	@sa IO::StorageState::placeholder
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
