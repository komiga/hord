/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Object definitions.
@ingroup object
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/serialization.hpp>
#include <Hord/IO/PropStateStore.hpp>

#include <duct/debug.hpp>
#include <duct/cc_unique_ptr.hpp>

#include <type_traits>
#include <functional>

namespace Hord {
namespace Object {

// Forward declarations
class Unit; // external
enum class UnitType : std::uint32_t;
enum class BaseType : std::uint32_t;
template<class>
struct unit_type_traits;
template<
	class,
	Object::BaseType const
>
struct unit_type_traits_impl;
template<class>
struct GenType;
template<Object::BaseType const>
struct GenID;
struct type_info;

/**
	@addtogroup object
	@{
*/

/**
	Maximum size of a slug.
*/
static constexpr std::size_t const
SLUG_MAX_SIZE = 0xFFu;

/*

M               32 bits               L
bbbb uuuu uuuu uuuu uuuu uuuu uuuu uuuu
   7    6    5    4    3    2    1    0
   ¦                                  ¦
   ·--- base type        unit type ---·

*/

/**
	Object type value type.
*/
using TypeValue = std::uint32_t;

/**
	Object base types.
*/
enum class BaseType : Object::TypeValue {
	/** Typeless. */
	null = 0u,
	/** @ref anchor. */
	Anchor,
	/** @ref rule. */
	Rule,
	/** @ref node. */
	Node
};

/**
	Bare object unit types.

	@note Unit type value of @c 0 is reserved to mean "no/null type".
	The range <code>[1, 2^8)</code> is reserved for standard unit
	types.

	@par
	@note Unless shadowing standard object units, user-defined unit
	type enum values must be in the range <code>[2^8, 2^28)</code>.
*/
enum class UnitType : Object::TypeValue {
	/** Typeless. */
	null = 0u
};

/**
	Owning pointer to object unit.
*/
using UPtr = duct::cc_unique_ptr<Object::Unit>;

/**
	Get the name of a base type.

	@returns C-string containing the name of @a base_type or
	"INVALID" if somehow @a base_type is not actually
	a @c Object::BaseType.
*/
inline char const*
get_base_type_name(
	Object::BaseType const base_type
) noexcept {
	switch (base_type) {
	case Object::BaseType::null:	return "null";
	case Object::BaseType::Anchor:	return "Anchor";
	case Object::BaseType::Rule:	return "Rule";
	case Object::BaseType::Node:	return "Node";
	}
	return "INVALID";
}

/**
	Unit type traits.

	Specializations of this class should derive from
	unit_type_traits_impl.

	@tparam T Unit type.
*/
template<class T>
struct unit_type_traits;

/**
	unit_type_traits implementation.

	@tparam T Unit type.
	@tparam B Base type value.
*/
template<
	class T,
	Object::BaseType const B
>
struct unit_type_traits_impl {
	/** Unit type. */
	using unit_type = T;

	/** Base type value. */
	static constexpr auto const base = B;
};

/** @cond INTERNAL */
template<>
struct unit_type_traits<Object::UnitType>
	: public Object::unit_type_traits_impl<
		Object::UnitType,
		Object::BaseType::null
	>
{};
/** @endcond */ // INTERNAL

/**
	Bare object type.

	@note A default-constructed object of this type will have
	a null base type and a null unit type.
*/
using Type = Object::GenType<Object::UnitType>;

/**
	Amorphic object type.

	@note This can morph to the base type implicitly and can downcast
	from Object::Type (the bare type) to a BaseType-specific type
	with Object::type_cast().

	@pre Object::unit_type_traits specialized for @a T.

	@tparam T Unit type.
*/
template<class T>
struct GenType final {
public:
	static_assert(
		std::is_enum<T>::value &&
		std::is_same<
			typename std::underlying_type<T>::type,
			Object::TypeValue
		>::value,
		"T must be an enum type with an underlying type"
		" of Object:TypeValue"
	);

	/** Unit type. */
	using unit_type = T;

	/** Unit type traits. */
	using traits_type = Object::unit_type_traits<unit_type>;

private:
	template<class>
	friend struct GenType;

	using this_type = GenType<T>;

	template<class T2>
	using this_rebind = GenType<T2>;

	enum : unsigned {
		shift_base = 28u,
		mask_base = 0xF0000000,
		mask_unit = 0x0FFFFFFF,
	};

	static constexpr Object::TypeValue
	make_value(
		Object::BaseType const base,
		unit_type const unit
	) noexcept {
		return
			(enum_cast(unit) & mask_unit) |
			(enum_cast(base) << shift_base)
		;
	}

	Object::TypeValue m_value{
		make_value(
			traits_type::base,
			static_cast<unit_type>(Object::UnitType::null)
		)
	};

	constexpr
	GenType(
		Object::TypeValue const value
	) noexcept
		: m_value(value)
	{}

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~GenType() noexcept = default;

	/**
		Default constructor.

		@post @code
			base() == traits_type::base &&
			is_null() == true
		@endcode
	*/
	constexpr GenType() = default;
	/** Copy constructor. */
	constexpr GenType(GenType const&) = default;
	/** Copy assignment operator. */
	GenType& operator=(GenType const&) = default;
	/** Move constructor. */
	constexpr GenType(GenType&&) = default;
	/** Move assignment operator. */
	GenType& operator=(GenType&&) = default;

	/**
		Constructor with base type.
	*/
	explicit constexpr
	GenType(
		Object::BaseType const base
	) noexcept
		: m_value(make_value(base, unit_type::null))
	{
		static_assert(
			std::is_same<this_type, Object::Type>::value,
			"cannot construct non-bare GenType with"
			" arbitrary base type"
		);
	}

	/**
		Constructor with base type and unit type.
	*/
	explicit constexpr
	GenType(
		Object::BaseType const base,
		unit_type const unit
	) noexcept
		: m_value(make_value(base, unit))
	{
		static_assert(
			std::is_same<this_type, Object::Type>::value,
			"cannot construct non-bare GenType with"
			" arbitrary base type"
		);
	}

	/**
		Constructor with unit type.
	*/
	explicit constexpr
	GenType(
		unit_type const unit
	) noexcept
		: m_value(make_value(traits_type::base, unit))
	{}
/// @}

/** @name Operators */ /// @{
	/**
		Equal-to operator.
	*/
	template<class T2>
	constexpr bool
	operator==(
		this_rebind<T2> const& rhs
	) const noexcept {
		return value() == rhs.value();
	}

	/**
		Not-equal-to operator.
	*/
	template<class T2>
	constexpr bool
	operator!=(
		this_rebind<T2> const& rhs
	) const noexcept {
		return value() != rhs.value();
	}

	/**
		Negation operator.

		@returns is_null().
	*/
	constexpr bool
	operator!() const noexcept {
		return is_null();
	}

	/**
		Bare type conversion operator.
	*/
	constexpr
	operator Object::Type() const noexcept {
		return Object::Type{m_value};
	}
/// @}

/** @name Properties */ /// @{
	/**
		Get base type.
	*/
	constexpr Object::BaseType
	base() const noexcept {
		return static_cast<Object::BaseType>(m_value >> shift_base);
	}

	/**
		Get unit type.
	*/
	constexpr unit_type
	unit() const noexcept {
		return static_cast<unit_type>(m_value & mask_unit);
	}

	/**
		Get value.

		@remarks This is the serial form.
	*/
	constexpr Object::TypeValue
	value() const noexcept {
		return m_value;
	}

	/**
		Check if either types are null.
	*/
	constexpr bool
	is_null() const noexcept {
		return
			!(m_value & mask_base) ||
			!(m_value & mask_unit)
		;
	}
/// @}

/** @name Operations */ /// @{
	/**
		Assign unit type.
	*/
	void
	assign(
		unit_type const unit
	) noexcept {
		m_value = make_value(base(), unit);
	}

	/**
		Assign to other base type and unit type.

		@note This function is enabled only on Object::Type (i.e.,
		with Object::UnitType).
	*/
	template<
		class T2,
		class = typename std::enable_if<
			std::is_same<this_type, Object::Type>::value,
			Object::unit_type_traits<T2>
		>::type
	>
	void
	assign(
		Object::BaseType const base,
		T2 const unit
	) noexcept {
		static_assert(
			std::is_same<this_type, Object::Type>::value,
			"cannot assign non-bare GenType to"
			" arbitrary base type"
		);
		m_value = make_value(base, unit);
	}
/// @}

/** @name Serialization */ /// @{
	static_assert(
		std::is_same<
			Object::TypeValue,
			std::uint32_t
		>::value,
		"schema changed"
	);

	/**
		Serialize.

		@warning State may not be retained if an exception is thrown.

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

/**
	Downcast object type.

	@pre Runtime assertion: @code
		type.base() == Object::GenType<T>::traits_type::base
	@endcode

	@tparam T Unit type.
*/
template<
	class T,
	class = typename std::enable_if<
		!std::is_same<Object::UnitType, T>::value
	>::type
>
inline Object::GenType<T>
type_cast(
	Object::Type const type
) {
	DUCT_ASSERT(
		type.base() == Object::GenType<T>::traits_type::base,
		"invalid object type cast: cannot downcast to different base type"
	);
	return Object::GenType<T>{
		static_cast<typename GenType<T>::unit_type>(type.unit())
	};
}

/**
	Null type.
*/
static constexpr Object::Type const
TYPE_NULL{};

/**
	Object ID value.
*/
using IDValue = std::uint32_t;

/**
	Bare object ID.
*/
using ID = Object::GenID<Object::BaseType::null>;

/** @cond INTERNAL */
enum : Object::IDValue {
	id_value_null = Object::IDValue{0},
};
/** @endcond */ // INTERNAL

/**
	Amorphic object ID.

	@note This can morph to the base ID type implicitly and can
	downcast from Object::ID (the bare ID type) to a BaseType-specific
	ID with Object::id_cast().

	@tparam B Base type.
*/
template<Object::BaseType const B>
struct GenID final {
public:
	/** Base type. */
	static constexpr Object::BaseType const
	base_type = B;

private:
	template<Object::BaseType const>
	friend struct GenID;

	using this_type = GenID<B>;

	template<Object::BaseType const B2>
	using this_rebind = GenID<B2>;

	Object::IDValue m_value{id_value_null};

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~GenID() noexcept = default;

	/**
		Default constructor.

		@post @code
			base() == traits_type::base &&
			is_null() == true
		@endcode
	*/
	constexpr GenID() = default;
	/** Copy constructor. */
	constexpr GenID(GenID const&) = default;
	/** Copy assignment operator. */
	GenID& operator=(GenID const&) = default;
	/** Move constructor. */
	constexpr GenID(GenID&&) = default;
	/** Move assignment operator. */
	GenID& operator=(GenID&&) = default;

	/**
		Generic copy constructor.

		@note This is effectively a downcast from Object::ID.
	*/
	template<
		Object::BaseType const B2,
		class = typename std::enable_if<
			Object::BaseType::null != B2 ||
			Object::BaseType::null != base_type
		>::type
	>
	explicit constexpr
	GenID(
		this_rebind<B2> const& id
	) noexcept
		: m_value(id.value())
	{}

	/**
		Constructor with ID value.
	*/
	explicit constexpr
	GenID(
		Object::IDValue const value
	) noexcept
		: m_value(value)
	{}
/// @}

/** @name Operators */ /// @{
	/**
		Equal-to operator.
	*/
	template<Object::BaseType const B2>
	constexpr bool
	operator==(
		this_rebind<B2> const& rhs
	) const noexcept {
		return value() == rhs.value();
	}

	/**
		Not-equal-to operator.
	*/
	template<Object::BaseType const B2>
	constexpr bool
	operator!=(
		this_rebind<B2> const& rhs
	) const noexcept {
		return value() != rhs.value();
	}

	/**
		Negation operator.

		@returns is_null().
	*/
	constexpr bool
	operator!() const noexcept {
		return is_null();
	}

	/**
		Bare type conversion operator.
	*/
	constexpr
	operator Object::ID() const noexcept {
		return Object::ID{m_value};
	}
/// @}

/** @name Properties */ /// @{
	/**
		Get value.

		@remarks This is the serial form.
	*/
	constexpr Object::IDValue
	value() const noexcept {
		return m_value;
	}

	/**
		Check if ID is null.
	*/
	constexpr bool
	is_null() const noexcept {
		return id_value_null == m_value;
	}

	/**
		Check if ID is reserved.

		@note ID_NULL is a reserved ID.
	*/
	constexpr bool
	is_reserved() const noexcept {
		return id_value_null == m_value;
	}
/// @}

/** @name Operations */ /// @{
	/**
		Assign value.
	*/
	void
	assign(
		IDValue const value
	) noexcept {
		m_value = value;
	}
/// @}

/** @name Serialization */ /// @{
	static_assert(
		std::is_same<
			Object::IDValue,
			std::uint32_t
		>::value,
		"schema changed"
	);

	/**
		Serialize.

		@warning State may not be retained if an exception is thrown.

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

static constexpr Object::ID const
/**
	Null ID.
*/
ID_NULL{Object::IDValue{id_value_null}};

/**
	Trait to test if a type is a GenID.
*/
template<class>
struct is_genid
	: public std::false_type
{};

/** @cond INTERNAL */
template<Object::BaseType const B>
struct is_genid<Object::GenID<B>>
	: public std::true_type
{};
/** @endcond */ // INTERNAL

/**
	Object type info.
*/
struct type_info {
/** @name Types */ /// @{
	/**
		Construct function type.
	*/
	using construct_type
	= Object::UPtr(
		Object::ID const id,
		Object::ID const parent
	);
/// @}

/** @name Properties */ /// @{
	/**
		Unit name.
	*/
	char const* const unit_name;

	/**
		Object type.

		@sa Object::Type
	*/
	Object::Type const type;

	/**
		Prop availability.
	*/
	IO::PropStateStore const props;

	/**
		Construct a unit of this type.

		@returns
		- The constructed object unit; or
		- @c nullptr if construction failed.
	*/
	construct_type& construct;
/// @}

/** @name Special member functions */ /// @{
	/**
		Constructor with properties.
	*/
	template<std::size_t const N>
	constexpr
	type_info(
		char const (&unit_name)[N],
		Object::Type const type,
		IO::PropStateStore const& props,
		construct_type& construct
	) noexcept
		: unit_name(unit_name)
		, type(type)
		, props(props)
		, construct(construct)
	{}
/// @}
};

/** @} */ // end of doc-group object

} // namespace Object
} // namespace Hord

/** @cond INTERNAL */
namespace std {
template<class T>
struct hash<Hord::Object::GenType<T>> {
	std::size_t
	operator()(
		Hord::Object::GenType<T> const type
	) const noexcept {
		return type.value();
	}
};

template<Hord::Object::BaseType const B>
struct hash<Hord::Object::GenID<B>> {
	std::size_t
	operator()(
		Hord::Object::GenID<B> const id
	) const noexcept {
		return id.value();
	}
};
} // namespace std
/** @endcond */ // INTERNAL
