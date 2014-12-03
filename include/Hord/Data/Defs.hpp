/**
@file
@brief Data definitions.
@ingroup data

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>

#include <utility>

namespace Hord {
namespace Data {

// Forward declarations
enum class FieldType : std::uint8_t;
enum class FieldMask : std::uint8_t;
struct FieldValue;

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
	Field value.
*/
struct FieldValue {
/** @name Properties */ /// @{
	/** Type. */
	Data::FieldType type{Data::FieldType::Text};

	/** Value. */
	String str{}; /**< String value. */
	union {
		std::int64_t num{0}; /**< Integer value. */
		bool bin; /**< Boolean value. */
	};
/// @}

/** @name Special member functions */ /// @{
	/** Destructor. */
	~FieldValue() noexcept = default;

	/** Default constructor. */
	FieldValue() = default;
	/** Copy constructor. */
	FieldValue(FieldValue const&) = default;
	/** Move constructor. */
	FieldValue(FieldValue&&) noexcept = default;
	/** Copy assignment operator. */
	FieldValue& operator=(FieldValue const&) = default;
	/** Move assignment operator. */
	FieldValue& operator=(FieldValue&&) noexcept = default;

	/**
		Construct with type (default value).
	*/
	FieldValue(
		Data::FieldType const type
	) noexcept
		: type(type)
	{}

	/**
		Construct with text value.
	*/
	FieldValue(
		char const* const value
	) noexcept
		: type(Data::FieldType::Text)
		, str(value)
	{}

	/**
		Construct with text value.
	*/
	FieldValue(
		String value
	) noexcept
		: type(Data::FieldType::Text)
		, str(std::move(value))
	{}

	/**
		Construct with number value.
	*/
	FieldValue(
		std::int64_t const value
	) noexcept
		: type(Data::FieldType::Number)
		, num(value)
	{}

	/**
		Construct with boolean value.
	*/
	FieldValue(
		bool const value
	) noexcept
		: type(Data::FieldType::Boolean)
		, num(value)
	{}
/// @}
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

/** @} */ // end of doc-group data

} // namespace Data
} // namespace Hord
