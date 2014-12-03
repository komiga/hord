/**
@file
@brief IO definitions.
@ingroup io

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/utility.hpp>
#include <Hord/serialization.hpp>

// TODO: docs for Rule

namespace Hord {
namespace IO {

// Forward declarations
enum class PropType : unsigned;
enum class PropTypeBit : unsigned;
enum class PropState : unsigned;
enum class StorageState : unsigned;
enum class Linkage : unsigned;

/**
	@addtogroup io
	@{
*/

/**
	Prop type.

	@note All props are supplied by an object unless otherwise stated.

	@par
	@note @c identity, @c metadata, and @c scratch are provided
	by Object::Unit (see PropTypeBit::base).

	@sa IO::PropTypeBit,
		IO::PropInfo,
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
		The following standard objects provide this prop:

		- Rule::Unit
		- Node::Unit
	*/
	primary,

	/**
		Auxiliary object data.

		This prop stores object data least likely to mutate.
		The following standard objects provide this prop:

		- Node::Unit
	*/
	auxiliary,

/** @cond INTERNAL */
	LAST
/** @endcond */
};

/**
	Prop type bitflags.

	@note These correspond to IO::PropType.
*/
enum class PropTypeBit : unsigned {
	/**
		Include identity prop in operation.
	*/
	identity = 1u << enum_cast(PropType::identity),

	/**
		Include metadata prop in operation.
	*/
	metadata = 1u << enum_cast(PropType::metadata),

	/**
		Include scratch space prop in operation.
	*/
	scratch = 1u << enum_cast(PropType::scratch),

	/**
		Include primary data prop in operation.
	*/
	primary = 1u << enum_cast(PropType::primary),

	/**
		Include auxiliary data prop in operation.
	*/
	auxiliary = 1u << enum_cast(PropType::auxiliary),

	/**
		Include base (implicit) props in operation.
	*/
	base
		= identity
		| metadata
		| scratch
	,

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
	,

	/**
		No props.
	*/
	none = 0u
};

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

		@sa Object::ID_NULL
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
	*/
	original,

	/**
		Differs from external storage.

		Runtime-side modifications not yet serialized.
	*/
	modified,

/** @cond INTERNAL */
	LAST
/** @endcond */
};

/**
	Object linkage.
*/
enum class Linkage : unsigned {
	/**
		Object is resident in datastore.
	*/
	resident = 0u,

	/**
		Object is not resident in datastore.
	*/
	orphan,

/** @cond INTERNAL */
	LAST
/** @endcond */
};

/**
	Get the name of a prop type.

	@returns C-string containing the name of @a prop_type or
	"INVALID" if somehow @a prop_type is not actually a IO::PropType.
*/
char const*
get_prop_type_name(
	IO::PropType const prop_type
) noexcept;

/**
	Get prop type as prop serialization bit.
*/
inline constexpr IO::PropTypeBit
prop_type_bit(
	IO::PropType const prop_type
) noexcept {
	return static_cast<IO::PropTypeBit>(
		1u << enum_cast(prop_type)
	);
}

/**
	Serialize PropType.
*/
template<class Ser>
inline ser_result_type
serialize(
	ser_tag_serialize,
	Ser& ser,
	IO::PropType& value
) {
	ser(Cacophony::make_enum_cfg<std::uint8_t>(const_safe<Ser>(value)));
}

/**
	Serialize PropTypeBit.
*/
template<class Ser>
inline ser_result_type
serialize(
	ser_tag_serialize,
	Ser& ser,
	IO::PropTypeBit& value
) {
	ser(Cacophony::make_enum_cfg<std::uint8_t>(const_safe<Ser>(value)));
}

/**
	Serialize Linkage.
*/
template<class Ser>
inline ser_result_type
serialize(
	ser_tag_serialize,
	Ser& ser,
	IO::Linkage& value
) {
	ser(Cacophony::make_enum_cfg<std::uint8_t>(const_safe<Ser>(value)));
}

/** @} */ // end of doc-group io

} // namespace IO
} // namespace Hord
