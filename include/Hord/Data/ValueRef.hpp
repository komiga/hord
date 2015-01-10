/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief ValueRef class.
@ingroup data
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/utility.hpp>
#include <Hord/String.hpp>
#include <Hord/Data/Defs.hpp>
#include <Hord/Object/Defs.hpp>

namespace Hord {
namespace Data {

// Forward declarations
struct ValueRef;

/**
	@addtogroup data
	@{
*/

/**
	Value reference.
*/
struct ValueRef {
public:
/** @name Properties */ /// @{
	/** Type. */
	Data::Type type{};
	/** Dynamic size. */
	unsigned size{0};
	/** Value data. */
	union ValueData {
		void const* dynamic;
		std::int8_t  s8;
		std::int16_t s16;
		std::int32_t s32;
		std::int64_t s64;
		std::uint8_t  u8;
		std::uint16_t u16;
		std::uint32_t u32;
		std::uint64_t u64;
		float f32;
		double f64;
		Object::ID object_id;
		char const* string;

		ValueData() noexcept : u64(0) {}
		ValueData(std::nullptr_t const) noexcept : ValueData() {}
		ValueData(void const* const data) noexcept : dynamic(data) {}
		ValueData(char const* const data) noexcept : string(data) {}
		ValueData(std::int8_t const value) noexcept : s8(value) {}
		ValueData(std::int16_t const value) noexcept : s16(value) {}
		ValueData(std::int32_t const value) noexcept : s32(value) {}
		ValueData(std::int64_t const value) noexcept : s64(value) {}
		ValueData(std::uint8_t const value) noexcept : u8(value) {}
		ValueData(std::uint16_t const value) noexcept : u16(value) {}
		ValueData(std::uint32_t const value) noexcept : u32(value) {}
		ValueData(std::uint64_t const value) noexcept : u64(value) {}
		ValueData(float const value) noexcept : f32(value) {}
		ValueData(double const value) noexcept : f64(value) {}
		ValueData(Object::ID const value) noexcept : object_id(value) {}
	} data{};
/// @}

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~ValueRef() noexcept = default;

	/** Default constructor. */
	ValueRef() = default;
	/** Copy constructor. */
	ValueRef(ValueRef const&) = default;
	/** Copy assignment operator. */
	ValueRef& operator=(ValueRef const&) = default;
	/** Move constructor. */
	ValueRef(ValueRef&&) = default;
	/** Move assignment operator. */
	ValueRef& operator=(ValueRef&&) = default;

	/**
		Construct empty value of type.
	*/
	ValueRef(
		Data::Type const type
	) noexcept
		: type(type)
	{}

	/**
		Construct with fixed-size value.
	*/
	template<class T>
	ValueRef(
		T const value
	) noexcept
		: type(Data::type_traits<T>::value())
		, size(0)
		, data(value)
	{}

	/**
		Construct with string value.

		@note @a data is assumed to be NUL-terminated.
	*/
	template<unsigned N>
	ValueRef(
		char const (&data)[N]
	) noexcept
		: type(Data::ValueType::string, Data::Size::b32)
		, size(N - 1)
		, data(data)
	{}

	/**
		Construct with string value.
	*/
	ValueRef(
		char const* const data,
		unsigned const size
	) noexcept
		: type(Data::ValueType::string, Data::Size::b32)
		, size(size)
		, data(data)
	{}

	/**
		Construct with string value.
	*/
	ValueRef(
		String const& str
	) noexcept
		: type(Data::ValueType::string, Data::Size::b32)
		, size(static_cast<unsigned>(str.size()))
		, data(str.data())
	{}
/// @}

/** @name Properties */ /// @{
	/**
		Get signed integer value.

		@returns @c 0 if the value is not an integer.
	*/
	std::int64_t
	integer_signed() const noexcept {
		if (type.type() != Data::ValueType::integer) {
			return 0;
		}
		if (!enum_bitand(type.flags(), Data::ValueFlag::integer_signed)) {
			return integer_unsigned();
		}
		switch (type.size()) {
		case Data::Size::b8 : return data.s8;
		case Data::Size::b16: return data.s16;
		case Data::Size::b32: return data.s32;
		case Data::Size::b64: return data.s64;
		}
	}

	/**
		Get unsigned integer value.

		@returns @c 0 if the value is not an integer.
	*/
	std::uint64_t
	integer_unsigned() const noexcept {
		if (type.type() != Data::ValueType::integer) {
			return 0;
		}
		if (enum_bitand(type.flags(), Data::ValueFlag::integer_signed)) {
			return integer_signed();
		}
		switch (type.size()) {
		case Data::Size::b8 : return data.u8;
		case Data::Size::b16: return data.u16;
		case Data::Size::b32: return data.u32;
		case Data::Size::b64: return data.u64;
		}
	}

	/**
		Get decimal value.

		@returns @c 0.0f if the value is not a decimal.
	*/
	double
	decimal() const noexcept {
		if (type.type() != Data::ValueType::decimal) {
			return 0.0f;
		} else if (type.size() == Data::Size::b64) {
			return data.f64;
		} else {
			return data.f32;
		}
	}
/// @}

/** @name Operators */ /// @{
	/**
		Equal-to operator.
	*/
	bool
	operator==(
		Data::ValueRef const& rhs
	) const noexcept;

	/**
		Not-equal-to operator.
	*/
	bool
	operator!=(
		Data::ValueRef const& rhs
	) const noexcept {
		return !(*this == rhs);
	}
/// @}

/** @name Operations */ /// @{
	/**
		Morph value to the specified type.

		@note Does nothing if type is dynamic.
	*/
	void
	morph(
		Data::Type const type
	) noexcept;

	/**
		Read value from NUL-terminated string.
	*/
	void
	read_from_string(
		unsigned const size,
		char const* const string
	) noexcept;
/// @}
};

/** @} */ // end of doc-group data

} // namespace Data
} // namespace Hord
