/**
@file Data/Defs.hpp
@brief Data definitions.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>

namespace Hord {
namespace Data {

// Forward declarations
enum class FieldType : std::uint8_t;
enum class MetaFieldType : std::uint8_t;

/**
	@addtogroup data
	@{
*/

/**
	Field type.

	@note The bitflag layout is only for Rule::type_info to
	restrict use; Fields must use a single value.

	@sa Data::Field,
		Data::FieldMask
*/
enum class FieldType : std::uint8_t {
	/** Textual field. */
	Text	= 1 << 0,

	/** Numeric field. */
	Number	= 1 << 1,

	/** Boolean field. */
	Boolean	= 1 << 2
};

/**
	Field type masks.

	@remarks Distinct type for restriction purposes
	(e.g., Node::Column).

	@sa Data::Field,
		Data::FieldType
*/
enum class FieldMask : std::uint8_t {
	/** Textual field. */
	Text	= enum_cast(Data::FieldType::Text),

	/** Numeric field. */
	Number	= enum_cast(Data::FieldType::Number),

	/** Boolean field. */
	Boolean	= enum_cast(Data::FieldType::Boolean),

	/** No types. */
	none = 0,

	/** All types. */
	any
		= FieldMask::Text
		| FieldMask::Number
		| FieldMask::Boolean
	,
};

/**
	@addtogroup metadata
	@{
*/

/**
	MetaField type.

	@sa Data::MetaField,
		Data::Metadata
*/
enum class MetaFieldType : std::uint8_t {
	/** Data::StringMetaField. */
	String = 0x01,

	/** Data::Int32MetaField. */
	Int32,

	/** Data::Int64MetaField. */
	Int64,

	/** Data::BoolMetaField. */
	Bool,

/** @cond INTERNAL */
	LAST
/** @endcond */
};

/**
	Serialize FieldType.
*/
template<class Ser>
inline ser_result_type
serialize(
	ser_tag_serialize,
	Ser& ser,
	Data::FieldType& value
) {
	ser(Cacophony::make_enum_cfg<std::uint8_t>(const_safe<Ser>(value)));
}

/**
	Serialize FieldMask.
*/
template<class Ser>
inline ser_result_type
serialize(
	ser_tag_serialize,
	Ser& ser,
	Data::FieldMask& value
) {
	ser(Cacophony::make_enum_cfg<std::uint8_t>(const_safe<Ser>(value)));
}

/** @} */ // end of doc-group metadata
/** @} */ // end of doc-group data

} // namespace Data
} // namespace Hord
