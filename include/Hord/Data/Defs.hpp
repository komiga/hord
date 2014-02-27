/**
@file Data/Defs.hpp
@brief Data definitions.

@author Tim Howard
@copyright 2013-2014 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_DATA_DEFS_HPP_
#define HORD_DATA_DEFS_HPP_

#include <Hord/config.hpp>

namespace Hord {
namespace Data {

// Forward declarations
enum class FieldType : uint8_t;
enum class MetaFieldType : uint8_t;

/**
	@addtogroup data
	@{
*/

/**
	Field type.

	@note The bitflag layout is only for Rule::type_info to
	restrict use; Fields must use a single value.

	@sa Data::Field
*/
enum class FieldType : uint8_t {
	/** Textual field. */
	Text	= 1 << 0,

	/** Numeric field. */
	Number	= 1 << 1,

	/** Boolean field. */
	Boolean	= 1 << 2
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
enum class MetaFieldType : uint8_t {
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

/** @} */ // end of doc-group metadata
/** @} */ // end of doc-group data

} // namespace Data
} // namespace Hord

#endif // HORD_DATA_DEFS_HPP_
