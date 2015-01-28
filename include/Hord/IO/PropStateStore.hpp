/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Prop state store.
@ingroup prop
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/utility.hpp>
#include <Hord/serialization.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/PropTypeIterator.hpp>

namespace Hord {
namespace IO {

// Forward declarations
class PropStateStore;

/**
	@addtogroup prop
	@{
*/

namespace {

// FIXME: For some terrific reason Clang sees this function as
// only being *declared* if it is placed in-class before the
// anonymous enum.
static constexpr unsigned
state_fill(
	IO::PropState const state
) noexcept {
	return 0u
		| (enum_cast(state) << (enum_cast(IO::PropType::identity ) << 2))
		| (enum_cast(state) << (enum_cast(IO::PropType::metadata ) << 2))
		| (enum_cast(state) << (enum_cast(IO::PropType::scratch  ) << 2))
		| (enum_cast(state) << (enum_cast(IO::PropType::primary  ) << 2))
		| (enum_cast(state) << (enum_cast(IO::PropType::auxiliary) << 2))
	;
}

} // anonymous namespace

/**
	Prop state store.

	@note Within the state store, all unsupplied props imply
	IO::PropState::original.

	@sa IO::PropState
*/
class PropStateStore final {
private:
	static_assert(
		5u == enum_cast(PropType::LAST),
		"masks and all_have() need to be updated"
	);
	static_assert(
		4u <= sizeof(unsigned),
		"implementation requires at least 4 bytes in unsigned int"
	);

	/*
		Bits (1-6 are props, s is whether they are supplied):
		XXss ssss 6666 5555 4444 3333 2222 1111

		6666 is currently unoccupied (there are only 5 props).
	*/
	enum : unsigned {
		// 4 bits per prop
		state_mask = 0x0F,
		// 5 props
		full_mask  = 0x000FFFFF,
		// 3 always-supplied props (identity, metadata, and scratch)
		base_value = 0x07000FFF,
		// With 32 bits, can store up to 6 props
		bit_mask   = 0x3F000000,

		// (PropType << type_shift) gives the number of bits
		// to move left for the prop's state
		type_shift = 2u,
		// (PropTypeBit << bit_shift) gives the
		// is_supplied bit for the prop
		bit_shift = 24u,

		bit_primary   = enum_cast(IO::PropTypeBit::primary) << bit_shift,
		bit_auxiliary = enum_cast(IO::PropTypeBit::auxiliary) << bit_shift,

		bit_primary_add
			= bit_primary
			| (state_mask << (enum_cast(IO::PropType::primary) << type_shift))
		,
		bit_auxiliary_add
			= bit_auxiliary
			| (state_mask << (enum_cast(IO::PropType::auxiliary) << type_shift))
		,

		prop_unsupplied_implied
			= enum_cast(IO::PropState::unsupplied)
			| enum_cast(IO::PropState::original)
		,
		prop_unsupplied_single = enum_cast(IO::PropState::unsupplied),

		mask_unsupplied_implied
			= state_fill(static_cast<IO::PropState>(prop_unsupplied_implied))
		,

		all_mask_unsupplied = state_fill(IO::PropState::unsupplied),
		all_mask_original   = state_fill(IO::PropState::original),
		all_mask_modified   = state_fill(IO::PropState::modified),
	};

	static constexpr unsigned
	shift_up(
		unsigned const value,
		IO::PropType const prop_type,
		unsigned const mask = state_mask
	) noexcept {
		return (value & mask) << (enum_cast(prop_type) << type_shift);
	}

	static constexpr unsigned
	shift_down(
		unsigned const value,
		IO::PropType const prop_type,
		unsigned const mask = state_mask
	) noexcept {
		return (value >> (enum_cast(prop_type) << type_shift)) & mask;
	}

	static constexpr unsigned
	mask_off(
		unsigned const value,
		IO::PropType const prop_type
	) noexcept {
		return ~(state_mask << (enum_cast(prop_type) << type_shift)) & value;
	}

	unsigned m_supplied;
	unsigned m_states;

	PropStateStore() = delete;
	PropStateStore& operator=(PropStateStore const&) = delete;

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~PropStateStore() = default;

	/** Copy constructor. */
	PropStateStore(PropStateStore const&) = default;
	/** Move constructor. */
	PropStateStore(PropStateStore&&) = default;
	/** Move assignment operator. */
	PropStateStore& operator=(PropStateStore&&) = default;

	/**
		Constructor with object data prop availability.

		@post All <strong>supplied</strong> props are uninitialized.

		@param supplies_primary Whether IO::PropType::primary is
		supplied.
		@param supplies_auxiliary Whether IO::PropType::auxiliary is
		supplied.
	*/
	constexpr
	PropStateStore(
		bool const supplies_primary,
		bool const supplies_auxiliary
	) noexcept
		: m_supplied(
			base_value
			| (supplies_primary ? bit_primary_add : 0u)
			| (supplies_auxiliary ? bit_auxiliary_add : 0u)
		)
		, m_states(
			~m_supplied & mask_unsupplied_implied
		)
	{}
/// @}

/** @name Properties */ /// @{
	/**
		Check if prop has a state.

		@note If @a state is a state combination, the return
		value will be @c true if <em>any</em> of the combined
		states are set.
	*/
	constexpr bool
	has(
		IO::PropType const prop_type,
		IO::PropState const state
	) const noexcept {
		return shift_down(m_states, prop_type, enum_cast(state));
	}

	/**
		Check if any prop is modified.
	*/
	constexpr bool
	any_modified() const noexcept {
		return 0u != (m_states & all_mask_modified);
	}

	/**
		Check if all props match external storage.
	*/
	constexpr bool
	all_original() const noexcept {
		return all_mask_original == (m_states & all_mask_original);
	}

	/**
		Check if all supplied props are uninitialized.
	*/
	constexpr bool
	all_uninitialized() const noexcept {
		return 0u == (m_states & m_supplied);
	}

	/**
		Check if all supplied data props are initialized.
	*/
	constexpr bool
	all_data_initialized() const noexcept {
		// NB: Non-supplied props "are" original within the store
		// and all other states imply the prop is initialized
		return shift_down(m_states, IO::PropType::primary)
			&& shift_down(m_states, IO::PropType::auxiliary);
	}

	/**
		Check if prop is initialized.
	*/
	constexpr bool
	is_initialized(
		IO::PropType const prop_type
	) const noexcept {
		return 0u != shift_down(m_states, prop_type);
	}

	/**
		Check if prop is supplied.
	*/
	constexpr bool
	supplies(
		IO::PropType const prop_type
	) const noexcept {
		return supplies_any(IO::prop_type_bit(prop_type));
	}

	/**
		Check if all given props are supplied.
	*/
	constexpr bool
	supplies_all(
		IO::PropTypeBit const props
	) const noexcept {
		return
			enum_cast(props)
			== (enum_cast(supplied()) & enum_cast(props))
		;
	}

	/**
		Check if any given props are supplied.
	*/
	constexpr bool
	supplies_any(
		IO::PropTypeBit const props
	) const noexcept {
		return enum_cast(supplied()) & enum_cast(props);
	}

	/**
		Get store value.
	*/
	constexpr unsigned
	value() const noexcept {
		return m_states;
	}

	/**
		Get full state for prop.
	*/
	constexpr IO::PropState
	state(
		IO::PropType const prop_type
	) const noexcept {
		return static_cast<IO::PropState>(
			shift_down(m_states, prop_type)
		);
	}

	/**
		Get supplied props.
	*/
	constexpr IO::PropTypeBit
	supplied() const noexcept {
		return static_cast<IO::PropTypeBit>(
			m_supplied >> bit_shift
		);
	}

	/**
		Get supplied props of a set of props.
	*/
	constexpr IO::PropTypeBit
	supplied_of(
		IO::PropTypeBit const prop_types
	) const noexcept {
		return static_cast<IO::PropTypeBit>(
			enum_cast(prop_types) & enum_cast(supplied())
		);
	}

	/**
		Get unsupplied props of a set of props.
	*/
	constexpr IO::PropTypeBit
	unsupplied_of(
		IO::PropTypeBit const prop_types
	) const noexcept {
		return static_cast<IO::PropTypeBit>(
			enum_cast(prop_types) & ~enum_cast(supplied())
		);
	}

	/**
		Get initialized props of a set of props.
	*/
	IO::PropTypeBit
	initialized_of(
		IO::PropTypeBit prop_types
	) const noexcept {
		return state_of(
			prop_types,
			IO::PropState::original |
			IO::PropState::modified
		);
	}

	/**
		Get uninitialized props of a set of props.
	*/
	IO::PropTypeBit
	uninitialized_of(
		IO::PropTypeBit prop_types
	) const noexcept {
		return not_state_of(
			prop_types,
			IO::PropState::original |
			IO::PropState::modified
		);
	}

	/**
		Get props with specific state of a set of props.

		@note This uses has(), so types that have any of the states
		in @a state will be selected.
	*/
	IO::PropTypeBit
	state_of(
		IO::PropTypeBit prop_types,
		IO::PropState const state
	) const noexcept {
		for (auto const type : prop_types) {
			if (!has(type, state)) {
				prop_types = static_cast<IO::PropTypeBit>(
					enum_cast(prop_types) & ~enum_cast(prop_type_bit(type))
				);
			}
		}
		return prop_types;
		/*return prop_types & (
			(has(IO::PropType::identity  , state)
			? IO::PropTypeBit::identity  : IO::PropTypeBit::none) |
			(has(IO::PropType::metadata  , state)
			? IO::PropTypeBit::metadata  : IO::PropTypeBit::none) |
			(has(IO::PropType::scratch   , state)
			? IO::PropTypeBit::scratch   : IO::PropTypeBit::none) |
			(has(IO::PropType::primary   , state)
			? IO::PropTypeBit::primary   : IO::PropTypeBit::none) |
			(has(IO::PropType::auxiliary , state)
			? IO::PropTypeBit::auxiliary : IO::PropTypeBit::none)
		);*/
	}

	/**
		Get props without specific state of a set of props.

		@note This uses has(), so types that have any of the states
		in @a state will be selected.
	*/
	IO::PropTypeBit
	not_state_of(
		IO::PropTypeBit prop_types,
		IO::PropState const state
	) const noexcept {
		for (auto const type : prop_types) {
			if (has(type, state)) {
				prop_types = static_cast<IO::PropTypeBit>(
					enum_cast(prop_types) & ~enum_cast(prop_type_bit(type))
				);
			}
		}
		return prop_types;
	}
/// @}

/** @name Operations */ /// @{
	/**
		Reset all states to uninitialized.

		@note Non-supplied states are not modified.

		@returns @c *this.
	*/
	PropStateStore&
	reset_all() noexcept {
		m_states = ~m_supplied & mask_unsupplied_implied;
		return *this;
	}

	/**
		Reset a prop state to uninitialized.

		@note If the prop is not supplied, this has no effect.

		@returns @c *this.
	*/
	PropStateStore&
	reset(
		PropType const prop_type
	) noexcept {
		if (supplies(prop_type)) {
			m_states = mask_off(m_states, prop_type);
		}
		return *this;
	}

	/**
		Assign prop state.

		@note If the prop is not supplied, this has no effect.

		@par
		@note IO::PropState::unsupplied is masked out
		of @a state before assignment. Only the constructor can
		assign this state.

		@returns @c *this.
	*/
	PropStateStore&
	assign(
		IO::PropType const prop_type,
		IO::PropState const state
	) noexcept {
		if (supplies(prop_type)) {
			m_states
				= mask_off(m_states, prop_type)
				| shift_up(
					~prop_unsupplied_single & enum_cast(state),
					prop_type
				)
			;
		}
		return *this;
	}

	/**
		Assign selected props to state.

		@returns @c *this.
	*/
	PropStateStore&
	assign(
		IO::PropTypeBit const prop_types,
		IO::PropState const state
	) noexcept {
		for (auto const type : prop_types) {
			assign(type, state);
		}
		return *this;
	}

	/**
		Assign all props to state.
	*/
	PropStateStore&
	assign_all(
		IO::PropState const state
	) noexcept {
		switch (state) {
		case IO::PropState::unsupplied:
			reset_all();
			break;
		case IO::PropState::original:
			m_states
				= (~m_supplied & mask_unsupplied_implied)
				| (m_supplied & all_mask_original)
			;
			break;
		case IO::PropState::modified:
			m_states
				= (~m_supplied & mask_unsupplied_implied)
				| (m_supplied & all_mask_modified)
			;
			break;
		}
		return *this;
	}
/// @}

/** @name Serialization */ /// @{
	/**
		Read from input serializer.

		@warning State may not be retained if an exception is thrown.

		@throws SerializerError{..}
		If a serialization operation failed.
	*/
	template<class Ser>
	ser_result_type
	read(
		ser_tag_read,
		Ser& ser
	) {
		ser(m_states);
		m_supplied
			= base_value
			| (supplies(IO::PropType::primary) ? bit_primary_add : 0u)
			| (supplies(IO::PropType::auxiliary) ? bit_auxiliary_add : 0u)
		;
		// Sanitize (unsupplied props have specific implied states)
		m_states |= ~m_supplied & mask_unsupplied_implied;
	}

	/**
		Write to output serializer.

		@throws SerializerError{..}
		If a serialization operation failed.
	*/
	template<class Ser>
	inline ser_result_type
	write(
		ser_tag_write,
		Ser& ser
	) const {
		std::uint32_t value
			= (m_supplied & bit_mask)
			| (m_states & full_mask)
		;
		ser(static_cast<std::uint32_t>(value));
	}
/// @}
};

/**
	Get beginning iterator for types.
*/
constexpr PropTypeIterator
begin(
	IO::PropStateStore const store
) noexcept {
	return {store.supplied()};
}

/**
	Get ending iterator for types.
*/
constexpr PropTypeIterator
end(
	IO::PropStateStore const /*store*/
) noexcept {
	return {PropTypeBit::none};
}

/** @} */ // end of doc-group prop

} // namespace IO
} // namespace Hord
