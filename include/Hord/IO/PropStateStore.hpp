/**
@file IO/PropStateStore.hpp
@brief Prop state store.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_IO_PROPSTATESTORE_HPP_
#define HORD_IO_PROPSTATESTORE_HPP_

#include <Hord/config.hpp>
#include <Hord/utility.hpp>
#include <Hord/IO/Defs.hpp>

namespace Hord {
namespace IO {

// Forward declarations
class PropStateStore;

/**
	@addtogroup io
	@{
*/
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
		// 5 states
		full_mask  = 0x000FFFFF,
		// 3 always-supplied props (identity, metadata, and scratch)
		base_value = 0x07000FFF,

		// (PropType << type_shift) gives the number of bits
		// to move left for the prop's state
		type_shift = 2u,
		// (PropTypeBit << flag_shift) gives the
		// is_supplied bit for the prop
		flag_shift = 24u,

		bit_primary
			= enum_cast(IO::PropTypeBit::primary)
			<< flag_shift
		,
		bit_auxiliary
			= enum_cast(IO::PropTypeBit::auxiliary)
			<< flag_shift
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
		all_mask_original = state_fill(IO::PropState::original),
		all_mask_modified = state_fill(IO::PropState::modified),
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

	unsigned m_states_supplied;
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
		: m_states_supplied(
			base_value
			| (!supplies_primary ? 0u : (
				bit_primary |
				shift_up(state_mask, IO::PropType::primary)
			))
			| (!supplies_auxiliary ? 0u : (
				bit_auxiliary |
				shift_up(state_mask, IO::PropType::auxiliary)
			))
		)
		, m_states(
			~m_states_supplied & mask_unsupplied_implied
		)
	{}
/// @}

/** @name Properties */ /// @{
	/**
		Check if prop has a state.

		@note If @a state is a state combination, the return
		value will be @c true if <em>any</em> of the combined
		states are set.

		@param prop_type Prop type.
		@param state Prop state.
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
		return 0u == (m_states & m_states_supplied);
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

		@param prop_type Prop type.
	*/
	constexpr bool
	is_initialized(
		IO::PropType const prop_type
	) const noexcept {
		return 0u != shift_down(m_states, prop_type);
	}

	/**
		Check if prop is supplied.

		@param prop_type Prop type.
	*/
	constexpr bool
	supplies(
		IO::PropType const prop_type
	) const noexcept {
		return supplies_all(IO::prop_flag(prop_type));
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
			== (enum_cast(get_supplied()) & enum_cast(props))
		;
	}

	/**
		Check if any given props are supplied.
	*/
	constexpr bool
	supplies_any(
		IO::PropTypeBit const props
	) const noexcept {
		return enum_cast(get_supplied()) & enum_cast(props);
	}

	/**
		Get store value.
	*/
	constexpr unsigned
	get_value() const noexcept {
		return m_states;
	}

	/**
		Get full state for prop.

		@param prop_type Prop type.
	*/
	constexpr IO::PropState
	get_state(
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
	get_supplied() const noexcept {
		return static_cast<IO::PropTypeBit>(
			m_states_supplied >> flag_shift
		);
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
		m_states = ~m_states_supplied & mask_unsupplied_implied;
		return *this;
	}

	/**
		Reset a prop state to uninitialized.

		@note If the prop is not supplied, this has no effect.

		@returns @c *this.
		@param prop_type Prop type.
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
		@param prop_type Prop type.
		@param state Prop state.
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
				= (~m_states_supplied & mask_unsupplied_implied)
				| (m_states_supplied & all_mask_original)
			;
			break;
		case IO::PropState::modified:
			m_states
				= (~m_states_supplied & mask_unsupplied_implied)
				| (m_states_supplied & all_mask_modified)
			;
			break;
		}
		return *this;
	}
/// @}
};

/** @} */ // end of doc-group prop
/** @} */ // end of doc-group io

} // namespace IO
} // namespace Hord

#endif // HORD_IO_PROPSTATESTORE_HPP_
