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
struct ValueTypeProperties;

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
	types (e.g., string) only use b64 as a storage hint.
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
value_type_first = Data::ValueType::null,
value_type_last = Data::ValueType::string;

constexpr auto const
size_first = Data::Size::b8,
size_last = Data::Size::b64;
/** @endcond */ // INTERNAL

/**
	Get the name of a value type.
*/
char const*
get_value_type_name(
	Data::ValueType const value_type
) noexcept;

/**
	Get the name of a data size.
*/
char const*
get_size_name(
	Data::Size const size
) noexcept;

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
		Data::Type const& rhs
	) const noexcept {
		return value() == rhs.value();
	}

	/**
		Not-equal-to operator.
	*/
	constexpr bool
	operator!=(
		Data::Type const& rhs
	) const noexcept {
		return value() != rhs.value();
	}

	/**
		Equal-to operator (ValueType).
	*/
	constexpr bool
	operator==(
		Data::ValueType const& rhs
	) const noexcept {
		return type() == rhs;
	}

	/**
		Not-equal-to operator (ValueType).
	*/
	constexpr bool
	operator!=(
		Data::ValueType const& rhs
	) const noexcept {
		return type() != rhs;
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
	Value type property flags.
*/
enum : unsigned {
	VTP_NONE = 0,
	VTP_DYNAMIC_SIZE = 1 << 0,
};

/**
	Value type properties.
*/
struct ValueTypeProperties {
	unsigned const flags;
	unsigned const fixed_size[4];
};

namespace {
constexpr static Data::ValueTypeProperties const
s_type_properties[]{
	// null
	{VTP_NONE, {0}},

	// dynamic
	{VTP_NONE, {0}},

	// integer
	{VTP_NONE, {1, 2, 4, 8}},

	// decimal
	{VTP_NONE, {4, 4, 4, 8}},

	// object_id
	{VTP_NONE, {sizeof(Object::IDValue)}},

	// string
	{VTP_DYNAMIC_SIZE, {0}},
};
} // anonymous namespace

/**
	Get value type properties for a value type.
*/
inline constexpr Data::ValueTypeProperties const&
type_properties(
	Data::ValueType const type
) noexcept {
	return Data::s_type_properties[enum_cast(type)];
}

/**
	Get value type properties for a type.
*/
inline constexpr Data::ValueTypeProperties const&
type_properties(
	Data::Type const type
) noexcept {
	return Data::type_properties(type.type());
}

/**
	Get meta data size in bytes.

	@note Data::Size::b32 is the maximum meta data size.
*/
inline constexpr unsigned
size_meta(
	Data::Size const size
) noexcept {
	return min_ce(4u, 1u << enum_cast(size));
}

/** @} */ // end of doc-group data

} // namespace Data
} // namespace Hord
