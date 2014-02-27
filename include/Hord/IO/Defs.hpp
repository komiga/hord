/**
@file IO/Defs.hpp
@brief IO definitions.

@author Tim Howard
@copyright 2013-2014 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_IO_DEFS_HPP_
#define HORD_IO_DEFS_HPP_

#include <Hord/config.hpp>

// TODO: docs for Rule

namespace Hord {
namespace IO {

// Forward declarations
enum class PropType : unsigned;
enum class PropState : unsigned;
enum class StorageState : unsigned;
enum class SerializationFlags : unsigned;

/**
	@addtogroup io
	@{
*/

/**
	Prop type.

	@note All props are supplied by an object unless otherwise stated.

	@par
	@note @c identity, @c metadata, and @c scratch are provided
	by Object::Unit.

	@sa IO::PropInfo,
		IO::Datastore
*/
enum class PropType : unsigned {
	/**
		Identity.

		This prop stores the object's parent and slug properties.
	*/
	identity = 0u,

	/**
		Metadata.

		This prop stores the object's metadata property.
	*/
	metadata,

	/**
		Scratch space.
	*/
	scratch,

	/**
		Primary object data.

		This prop stores object data most likely to mutate.
		The following objects provide this prop:

		- Hive::Unit
		- Rule::Unit
		- Node::Unit
	*/
	primary,

	/**
		Auxiliary object data.

		This prop stores object data least likely to mutate.
		The following objects provide this prop:

		- Node::Unit
	*/
	auxiliary,

/** @cond INTERNAL */
	LAST
/** @endcond */
};

/**
	Get the name of a prop type.

	@returns C-string containing the name of @a prop_type or
	"INVALID" if somehow @a prop_type is not actually a IO::PropType.
	@param prop_type Prop type.
*/
char const*
get_prop_type_name(
	IO::PropType const prop_type
) noexcept;

/**
	Prop state.

	@sa IO::PropStateStore
*/
enum class PropState : unsigned {
	/**
		Prop not supplied by object.

		@note This includes IO::PropState::original.
	*/
	unsupplied = 1 << 0,

	/**
		Prop matches external storage.
	*/
	original = 1 << 1,

	/**
		Prop differs from external storage.
	*/
	modified = 1 << 2
};

/**
	@ref object "Object" storage state.
*/
enum class StorageState : unsigned {
	/**
		Null/invalid object.

		Object has no identifying information, a state in which
		neither serialization nor deserialization can be performed.

		@sa Object::NULL_ID
	*/
	null = 0u,

	/**
		Placeholder.

		Object has only sufficient identifying information
		(ID property) to allow deserialization.
	*/
	placeholder,

	/**
		Fully identified placeholder.

		Object has non-empty slug and typically has an assigned
		parent.
	*/
	placeholder_identified,

	/**
		Fully matches external storage.

		Specifically, after successful serialization or
		deserialization.

		@note For hives, this means that the runtime only
		has identifying information for all children, not that all
		children are fully deserialized. In a typical configuration,
		children are fully deserialized on demand, but always at
		least placeheld.
	*/
	original,

	/**
		Differs from external storage.

		Runtime-side modifications not yet serialized.

		@note Hives will only have this state when:
		-# one of its base props are modified (IO::PropType::identity
		   or IO::PropType::metadata); or
		-# when a child is added or removed (IO::PropType::primary).
		@note
		If a child object is modified, it does not affect the
		hive's state.
	*/
	modified,

/** @cond INTERNAL */
	LAST
/** @endcond */
};

/**
	Prop serialization flags.

	@note These correspond to IO::PropType.
*/
enum class PropSerializationFlags : unsigned {
	/**
		Include identity prop in operation.
	*/
	identity = 1 << 0,

	/**
		Include metadata prop in operation.
	*/
	metadata = 1 << 1,

	/**
		Include scratch space prop in operation.
	*/
	scratch = 1 << 2,

	/**
		Include primary data prop in operation.
	*/
	primary = 1 << 3,

	/**
		Include auxiliary data prop in operation.
	*/
	auxiliary = 1 << 4,

	/**
		Include identity and metadata props in operation.

		@remarks This is only used by the driver when deserializing
		placeheld objects.
	*/
	shallow
		= identity
		| metadata
	,

	/**
		Include primary and auxiliary data props in operation.
	*/
	data
		= primary
		| auxiliary
	,

	/**
		All props.
	*/
	all
		= identity
		| metadata
		| scratch
		| primary
		| auxiliary
};

/** @} */ // end of doc-group io

} // namespace IO
} // namespace Hord

#endif // HORD_IO_DEFS_HPP_
