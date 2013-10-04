/**
@file IO/Defs.hpp
@brief IO definitions.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_IO_DEFS_HPP_
#define HORD_IO_DEFS_HPP_

#include <Hord/config.hpp>

namespace Hord {
namespace IO {

// Forward declarations
enum class PropType : unsigned;
enum class StorageState : unsigned;
enum class SerializationFlags : unsigned;

/**
	@addtogroup io
	@{
*/

/**
	Prop type.

	@sa IO::PropInfo,
		IO::Datastore
*/
enum class PropType : unsigned {
	/**
		Object metadata.
	*/
	object_metadata = 0u,

	/**
		Primary object data.
	*/
	object_primary,

	/**
		Auxiliary object data.
	*/
	object_auxiliary,

	/**
		Scratch space.
	*/
	object_scratch,

/** @cond INTERNAL */
	LAST
/** @endcond */
};

/**
	@ref object storage state.
*/
enum class StorageState : unsigned {
	/**
		Null/invalid object.

		%Object has no identifying information, a state in which
		neither serialization nor deserialization can be performed.
		@sa Object::NULL_ID
	*/
	null = 0u,

	/**
		Placeholder.

		%Object has identifying information, a state in which
		deserialization can be performed (but not serialization).

		@note An object can have this state when its metadata
		property is deserialized. Most objects only change
		to @c original when their primary data is deserialized.
	*/
	placeholder,

	/**
		Matches external storage.

		Specifically, after successful serialization or
		deserialization.

		@note For hives, this implicitly means that the runtime only
		has identifying information for all children -- not that all
		children are fully deserialized. In a typical configuration,
		children are fully (de)serialized on demand, and always
		placeheld.
	*/
	original,

	/**
		Modified.

		Runtime-side modifications not yet serialized.

		@note %Hives will only have this state when:
		-# one of its stored properties is modified (metadata or
		   slug); or
		-# when a child is added or removed.
		If a child object is modified, it does not affect the hive's
		state.
	*/
	modified,

/** @cond INTERNAL */
	LAST
/** @endcond */
};

/**
	Serialization flags.

	@note All of these flags apply both to serialization
	and deserialization unless otherwise stated.
*/
enum class SerializationFlags : unsigned {
	/**
		Include identifying information in operation.

		Identifying information includes Object's owner, ID, and slug
		properties. Only the owner and ID properties are required to
		deserialize an object.

		On non-hive objects, this flag will only (de)serialize the
		object's slug (the object's actual ID is supplied by the
		hive's primary data).

		Post-@c placeholder state, this flag is only significant when
		serializing a hive after a child was added or removed.

		@note This flag is implicit.
	*/
	identity = 1 << 0,

	/**
		Include metadata property in operation.
	*/
	metadata = 1 << 1,

	/**
		Include primary data in operation.

		%Hives' primary data is its idset property. A child will only
		be placeheld when its owner's primary data is deserialized.
	*/
	primary = 1 << 2,

	/**
		Include auxiliary data in operation.

		@remarks This corresponds to the @c object_auxiliary prop,
		which only nodes use.
	*/
	auxiliary = 1 << 3,

	/**
		Include scratch space in operation.
	*/
	scratch = 1 << 4,

	/**
		Shallow deserialization.

		Includes both @c identity and @c metadata.

		@remarks This flag is only used by the driver when
		deserializing placeheld objects.
	*/
	shallow
		= identity
		| metadata
	,

	/**
		All flags.
	*/
	all
		= identity
		| metadata
		| primary
};

/** @} */ // end of doc-group io

} // namespace IO
} // namespace Hord

#endif // HORD_IO_DEFS_HPP_
