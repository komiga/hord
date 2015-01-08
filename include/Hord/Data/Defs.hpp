/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Data definitions.
@ingroup data
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/utility.hpp>
#include <Hord/String.hpp>
#include <Hord/serialization.hpp>
#include <Hord/Object/Defs.hpp>

#include <type_traits>

namespace Hord {
namespace Data {

// Forward declarations
enum class ValueType : std::uint8_t;
enum class ValueFlag : std::uint8_t;
enum class Size : std::uint8_t;
struct Type;
struct ValueRef;

/**
	@addtogroup data
	@{
*/

/**
	Type value type.
*/
using TypeValue = std::uint8_t;

/**
	Data value types.
*/
enum class ValueType : Data::TypeValue {
	/**
		Empty type.

		@note This is for special purposes.
	*/
	null,

	/** Type-stored value. */
	dynamic,

	/** Integer. */
	integer,

	/** Floating-point type. */
	decimal,

	/** Object ID. */
	object_id,

	/** String. */
	string,
};

/**
	Data value flags.
*/
enum class ValueFlag : Data::TypeValue {
	/** Empty flag. */
	none = 0u,

	/** Make ValueType::integer signed. */
	integer_signed = 1 << 0,
};

/**
	Data value size in bits.

	@note Size has no effect on @c ValueType::object_id.
	Size is a storage hint for @c ValueType::dynamic. Variably-sized
	types (e.g., string) only use b64 as a storage hint. The maximum
	variably-sized size value is b32.
*/
enum class Size : Data::TypeValue {
	/** 8 bits. */
	b8,
	/** 16 bits. */
	b16,
	/** 32 bits. */
	b32,
	/** 64 bits. */
	b64,
};

/** @cond INTERNAL */
constexpr auto const
value_type_first{Data::ValueType::null},
value_type_last{Data::ValueType::object_id};
/** @endcond */ // INTERNAL

/**
	Type.
*/
struct Type {
private:
	enum : unsigned {
		mask_type = 0x07,
		mask_flags = 0x07,
		shift_flags = 4,
		shift_size = 6,
	};

	static constexpr Data::TypeValue
	make_value(
		Data::ValueType const value_type,
		Data::ValueFlag const value_flags,
		Data::Size const size
	) noexcept {
		return
			(enum_cast(value_type)) |
			(enum_cast(value_flags) << shift_flags) |
			(enum_cast(size) << shift_size)
		;
	}

	enum : Data::TypeValue {
		value_null
			= (enum_cast(Data::ValueType::null))
			| (enum_cast(Data::ValueFlag::none) << shift_flags)
			| (enum_cast(Data::Size::b8) << shift_size)
		,
	};

	Data::TypeValue m_value{value_null};

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~Type() noexcept = default;

	/** Default constructor. */
	constexpr Type() = default;
	/** Copy constructor. */
	constexpr Type(Type const&) = default;
	/** Copy assignment operator. */
	Type& operator=(Type const&) = default;
	/** Move constructor. */
	constexpr Type(Type&&) = default;
	/** Move assignment operator. */
	Type& operator=(Type&&) = default;

	/**
		Construct with data value type and size.
	*/
	constexpr
	Type(
		Data::ValueType const value_type,
		Data::Size const size = Data::Size::b8
	) noexcept
		: m_value(make_value(value_type, Data::ValueFlag::none, size))
	{}

	/**
		Construct with data value type, flags, and size.
	*/
	constexpr
	Type(
		Data::ValueType const value_type,
		Data::ValueFlag const value_flags,
		Data::Size const size = Data::Size::b8
	) noexcept
		: m_value(make_value(value_type, value_flags, size))
	{}
/// @}

/** @name Operators */ /// @{
	/**
		Equal-to operator.
	*/
	constexpr bool
	operator==(
		Type const& rhs
	) const noexcept {
		return value() == rhs.value();
	}

	/**
		Not-equal-to operator.
	*/
	constexpr bool
	operator!=(
		Type const& rhs
	) const noexcept {
		return value() != rhs.value();
	}
/// @}

/** @name Properties */ /// @{
	/**
		Get value type.
	*/
	constexpr Data::ValueType
	type() const noexcept {
		return static_cast<Data::ValueType>(m_value & mask_type);
	}

	/**
		Get value flags.
	*/
	constexpr Data::ValueFlag
	flags() const noexcept {
		return static_cast<Data::ValueFlag>((m_value >> shift_flags) & mask_flags);
	}

	/**
		Get value size.
	*/
	constexpr Data::Size
	size() const noexcept {
		return static_cast<Data::Size>(m_value >> shift_size);
	}

	/**
		Get value.

		@remarks This is the serial form.
	*/
	constexpr Data::TypeValue
	value() const noexcept {
		return m_value;
	}

	/**
		Set value.

		@remarks This is the serial form.
	*/
	void
	set_value(
		Data::TypeValue const value
	) noexcept {
		m_value = value;
	}
/// @}

/** @name Serialization */ /// @{
	/**
		Serialize.

		@throws SerializerError{..}
		If a serialization operation failed.
	*/
	template<class Ser>
	ser_result_type
	serialize(
		ser_tag_serialize,
		Ser& ser
	) {
		auto& self = const_safe<Ser>(*this);
		ser(self.m_value);
	}
/// @}
};

/** @cond INTERNAL */
namespace {

template<class>
struct type_traits_impl {
	static constexpr Data::Type
	value() { return {Data::ValueType::null}; }
};

#define HORD_DEFINE_TYPE_TO_DATA_TYPE(T, VT, VF, SZ)				\
	template<> struct type_traits_impl<T> {							\
		static constexpr Data::Type									\
		value() {													\
			return {												\
				Data::ValueType:: VT,								\
				Data::ValueFlag:: VF,								\
				Data::Size:: SZ										\
			};														\
		}															\
	} //

HORD_DEFINE_TYPE_TO_DATA_TYPE(std::int8_t , integer, integer_signed, b8);
HORD_DEFINE_TYPE_TO_DATA_TYPE(std::int16_t, integer, integer_signed, b16);
HORD_DEFINE_TYPE_TO_DATA_TYPE(std::int32_t, integer, integer_signed, b32);
HORD_DEFINE_TYPE_TO_DATA_TYPE(std::int64_t, integer, integer_signed, b64);

HORD_DEFINE_TYPE_TO_DATA_TYPE(std::uint8_t , integer, none, b8);
HORD_DEFINE_TYPE_TO_DATA_TYPE(std::uint16_t, integer, none, b16);
HORD_DEFINE_TYPE_TO_DATA_TYPE(std::uint32_t, integer, none, b32);
HORD_DEFINE_TYPE_TO_DATA_TYPE(std::uint64_t, integer, none, b64);

HORD_DEFINE_TYPE_TO_DATA_TYPE(float , decimal, none, b32);
HORD_DEFINE_TYPE_TO_DATA_TYPE(double, decimal, none, b64);

#undef HORD_DEFINE_TYPE_TO_DATA_TYPE

template<Object::BaseType const B>
struct type_traits_impl<Object::GenID<B>> {
	static constexpr Data::Type const
	value() { return {Data::ValueType::object_id}; }
};

} // anonymous namespace
/** @endcond */ // INTERNAL

/**
	Get data type for type.

	@note @c value will be Data::Type::null if the type does not have
	a conversion to Data::Type.
*/
template<class T>
struct type_traits {
	static constexpr Data::Type
	value() {
		return {Data::type_traits_impl<typename std::remove_const<T>::type>::value()};
	}
};

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
};

/** @} */ // end of doc-group data

} // namespace Data
} // namespace Hord
