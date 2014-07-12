/**
@file IO/PropTypeIterator.hpp
@brief PropType iterator.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/utility.hpp>
#include <Hord/IO/Defs.hpp>

#include <duct/utility.hpp>

#include <type_traits>

namespace Hord {
namespace IO {

// Forward declarations
class PropTypeIterator;

/**
	@addtogroup io
	@{
*/
/**
	@addtogroup prop
	@{
*/

/** @cond INTERNAL */
namespace {
static constexpr unsigned
s_bit_position[]{
	enum_cast(IO::PropType::LAST), // none
	enum_cast(IO::PropType::identity),
	enum_cast(IO::PropType::metadata),
	0u,
	enum_cast(IO::PropType::scratch),
	0u,0u,0u,
	enum_cast(IO::PropType::primary),
	0u,0u,0u,0u, 0u,0u,0u,
	enum_cast(IO::PropType::auxiliary)
};
static_assert(
	17u == std::extent<decltype(s_bit_position)>::value,
	"s_bit_position needs to be updated"
);

#define assert_bit(t_)										\
	static_assert(											\
		s_bit_position[enum_cast(IO::PropTypeBit:: t_)]		\
		== enum_cast(IO::PropType:: t_),					\
		"bit is not in the correct location"				\
	)

assert_bit(identity);
assert_bit(metadata);
assert_bit(scratch);
assert_bit(primary);
assert_bit(auxiliary);

#undef assert_bit

} // anonymous namespace
/** @endcond */ // INTERNAL

/**
	PropType iterator.
*/
class PropTypeIterator {
private:
	enum : unsigned {
		end_position = enum_cast(IO::PropType::LAST),
		mask_value = enum_cast(IO::PropTypeBit::all),
	};

	static constexpr unsigned
	lsb(
		unsigned const value
	) {
		return value & unsigned(-signed(value));
	}

	unsigned m_value;
	unsigned m_position;

	PropTypeIterator() = delete;

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~PropTypeIterator() = default;

	/** Copy constructor. */
	PropTypeIterator(PropTypeIterator const&) = default;
	/** Move constructor. */
	PropTypeIterator(PropTypeIterator&&) = default;
	/** Copy assignment operator. */
	PropTypeIterator& operator=(PropTypeIterator const&) = default;
	/** Move assignment operator. */
	PropTypeIterator& operator=(PropTypeIterator&&) = default;

	/**
		Constructor with types.
	*/
	constexpr
	PropTypeIterator(
		IO::PropTypeBit const types
	) noexcept
		: m_value(enum_cast(types) & mask_value)
		, m_position(
			// Woo paranoia
			s_bit_position[mask_value & lsb(m_value)]
		)
	{}
/// @}

/** @name Properties */ /// @{
	/**
		Get value.
	*/
	constexpr IO::PropTypeBit
	value() const noexcept {
		return static_cast<IO::PropTypeBit>(m_value);
	}

	/**
		Get position.
	*/
	constexpr unsigned
	position() const noexcept {
		return m_position;
	}

	/**
		Check if iterator has a type at its position.
	*/
	constexpr bool
	has() const noexcept {
		return m_value & (1u << m_position);
	}

	/**
		Get type at current position.
	*/
	constexpr IO::PropType
	type() const noexcept {
		return has()
			? static_cast<IO::PropType>(m_position)
			: static_cast<IO::PropType>(-1)
		;
	}
/// @}

/** @name Operators */ /// @{
	/**
		Indirection operator.
	*/
	IO::PropType
	operator*() const noexcept {
		return type();
	}

	/**
		Post-increment operator.
	*/
	PropTypeIterator
	operator++(int) noexcept {
		PropTypeIterator copy{*this};
		++(*this);
		return copy;
	}

	/**
		Pre-increment operator.
	*/
	PropTypeIterator&
	operator++() noexcept {
		m_position = s_bit_position[
			lsb(m_value & (mask_value << (1u + m_position)))
		];
		return *this;
	}

	/**
		Equal-to operator.
	*/
	bool
	operator==(
		PropTypeIterator const& other
	) const noexcept {
		return m_position == other.m_position;
	}

	/**
		Not-equal-to operator.
	*/
	bool
	operator!=(
		PropTypeIterator const& other
	) const noexcept {
		return m_position != other.m_position;
	}
/// @}
};

/**
	Get beginning iterator for types.
*/
constexpr PropTypeIterator
begin(
	IO::PropTypeBit const types
) noexcept {
	return {types};
}

/**
	Get ending iterator for types.
*/
constexpr PropTypeIterator
end(
	IO::PropTypeBit const /*types*/
) noexcept {
	return {PropTypeBit::none};
}

/** @} */ // end of doc-group prop
/** @} */ // end of doc-group io

} // namespace IO
} // namespace Hord
