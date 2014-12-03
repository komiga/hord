/**
@file IO/Ops.hpp
@brief IO operations.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/Object/Unit.hpp>
#include <Hord/IO/Datastore.hpp>

namespace Hord {
namespace IO {

// Forward declarations

/**
	@addtogroup io
	@{
*/

/**
	Load a prop.

	@note Unless @a force is @c true, this will only load the prop
	if it is uninitialized.

	@post With no exceptions:
	@code
		object.get_prop_states().has(
			prop_type,
			IO::PropState::original
		)
	@endcode

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
	Load a prop (weak).

	Same as IO::load_prop(), but with weak acquisition.
*/
bool
load_prop_weak(
	IO::Datastore& datastore,
	Object::Unit& object,
	IO::PropType const prop_type,
	std::istream& stream,
	bool const force = false
);

/**
	Load props.

	@note Unless @a force is @c true, this will only load the props
	if they are uninitialized.

	@throws Error{...}
	From Object::Unit::deserialize().

	@returns The number of loaded props.
	@param datastore %Datastore.
	@param object Object to operate on.
	@param prop_types Props to load.
	@param force Whether to force load, disregarding prop state.

	@sa Object::Unit::deserialize()
*/
unsigned
load_props(
	IO::Datastore& datastore,
	Object::Unit& object,
	IO::PropTypeBit const prop_types,
	bool const force = false
);

/**
	Load props (weak).

	Same as IO::load_props(), but with weak acquisition.

	@warning Prop data in @a stream must be ordered by prop type
	order.
*/
unsigned
load_props_weak(
	IO::Datastore& datastore,
	Object::Unit& object,
	IO::PropTypeBit const prop_types,
	std::istream& stream,
	bool const force = false
);

/**
	Store a prop.

	@note Unless @a force is @c true, this will only store the prop
	if it is modified.

	@post With no exceptions:
	@code
		object.get_prop_states().has(
			prop_type,
			IO::PropState::original
		)
	@endcode

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
	Store a prop (weak).

	Same as IO::store_prop(), but with weak acquisition.
*/
bool
store_prop_weak(
	IO::Datastore& datastore,
	Object::Unit& object,
	IO::PropType const prop_type,
	std::ostream& stream,
	bool const force = false
);

/**
	Store props.

	@note Unless @a force is @c true, this will only store the props
	if they are modified.

	@throws Error{...}
	From Object::Unit::serialize().

	@returns The number of props stored.
	@param datastore %Datastore.
	@param object Object.
	@param prop_types Props to store.
	@param force Whether to force store, disregarding prop state.

	@sa Object::Unit::serialize()
*/
unsigned
store_props(
	IO::Datastore& datastore,
	Object::Unit& object,
	IO::PropTypeBit const prop_types,
	bool const force = false
);

/**
	Store props (weak).

	Same as IO::store_props(), but with weak acquisition.
*/
unsigned
store_props_weak(
	IO::Datastore& datastore,
	Object::Unit& object,
	IO::PropTypeBit const prop_types,
	std::ostream& stream,
	bool const force = false
);

/** @} */ // end of doc-group io

} // namespace IO
} // namespace Hord
