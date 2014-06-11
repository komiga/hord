/**
@file Cmd/Defs.hpp
@brief Command definitions.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_CMD_DEFS_HPP_
#define HORD_CMD_DEFS_HPP_

#include <Hord/config.hpp>
#include <Hord/utility.hpp>
#include <Hord/serialization.hpp>

#include <duct/cc_unique_ptr.hpp>
#include <duct/StateStore.hpp>

#include <type_traits>

namespace Hord {
namespace Cmd {

// Forward declarations
class Unit; // external
class Stage; // external
struct ID;
struct type_info;
struct type_info_table;
enum class Type : std::uint32_t;
enum class Flags : std::uint32_t;
enum class StageType : std::uint8_t;
enum class Status : unsigned;

/**
	@addtogroup cmd
	@{
*/

/** @cond INTERNAL */
// Forgive me
#define HORD_CMD_JOIN_CT_(cmdn_) \
	HORD_CMD_JOIN_CT_I_(cmdn_)
#define HORD_CMD_JOIN_CT_I_(cmdn_) \
	s_type_info_ ## cmdn_

#define HORD_CMD_JOIN_ST_(cmdn_, stagen_) \
	HORD_CMD_JOIN_ST_I_(cmdn_, stagen_)
#define HORD_CMD_JOIN_ST_I_(cmdn_, stagen_) \
	s_type_info_ ## cmdn_ ## _ ## stagen_
/** @endcond */ // INTERNAL

/**
	%ID value.

	@note Value only takes up bits @c 0x3FFFFFFF.
	See @c Cmd::ID.
*/
using IDValue = std::uint32_t;

/**
	Owning pointer to command unit.
*/
using UnitUPtr = duct::cc_unique_ptr<Cmd::Unit>;
/**
	Owning pointer to command stage.
*/
using StageUPtr = duct::cc_unique_ptr<Cmd::Stage>;

namespace {
enum : Cmd::IDValue {
	id_value_null = Cmd::IDValue{0u},
	id_value_max = Cmd::IDValue{0x3FFFFFFFu},
};
}

/**
	%ID.
*/
struct ID final {
private:
	enum : unsigned {
		shift_host		= 30u,
		shift_initiator	= 31u,
		mask_base		= 0x3FFFFFFFu,
		mask_canonical	= 0x7FFFFFFFu,
		mask_host		= unsigned{1u << shift_host},
		mask_initiator	= unsigned{1u << shift_initiator},
	};

	Cmd::IDValue m_value{id_value_null};

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~ID() noexcept = default;

	/** Default constructor. */
	ID() = default;
	/** Copy constructor. */
	ID(ID const&) = default;
	/** Copy assignment operator. */
	ID& operator=(ID const&) = default;
	/** Move constructor. */
	ID(ID&&) = default;
	/** Move assignment operator. */
	ID& operator=(ID&&) = default;

	/**
		Constructor with value.
	*/
	explicit constexpr
	ID(
		Cmd::IDValue const value
	) noexcept
		: m_value(value)
	{}

	/**
		Constructor with base and host.
	*/
	explicit constexpr
	ID(
		Cmd::IDValue const base,
		bool const host
	) noexcept
		: ID(base, host, false)
	{}

	/**
		Constructor with properties.
	*/
	explicit constexpr
	ID(
		Cmd::IDValue const base,
		bool const host,
		bool const initiator
	) noexcept
		: m_value(
			  (base & mask_base)
			| (Cmd::IDValue{host} << shift_host)
			| (Cmd::IDValue{initiator} << shift_initiator)
		)
	{}
/// @}

/** @name Properties */ /// @{
	/**
		Get base value.
	*/
	constexpr Cmd::IDValue
	base() const noexcept {
		return m_value & mask_base;
	}

	/**
		Get canonical value.

		@note @c fields.flag_host is included in the canonical
		value.
	*/
	constexpr Cmd::IDValue
	canonical() const noexcept {
		return m_value & mask_canonical;
	}

	/**
		Get value.

		@remarks This is the serial form.
	*/
	constexpr Cmd::IDValue
	value() const noexcept {
		return m_value;
	}

	/**
		Check if the base %ID value is non-null.
	*/
	constexpr bool
	is_identified() const noexcept {
		// NB: Avoid sticky flag_host by checking the base value
		return id_value_null != base();
	}

	/**
		Check if the command is host-initiated.
	*/
	constexpr bool
	is_host() const noexcept {
		return m_value & mask_host;
	}

	/**
		Check if the command is client-initiated.
	*/
	constexpr bool
	is_client() const noexcept {
		return m_value ^ mask_host;
	}

	/**
		Check if the stage is an initiator.

		@note This is enabled for the first result stage of a
		local command. It ensures there are no stray command
		initiations, and assists when resolving %ID collisions.
	*/
	constexpr bool
	is_initiator() const noexcept {
		return m_value & mask_initiator;
	}
/// @}

/** @name Operations */ /// @{
	/**
		Assign value.
	*/
	void
	assign(
		Cmd::IDValue const value
	) noexcept {
		m_value = value;
	}

	/**
		Assign fields.
	*/
	void
	assign(
		Cmd::IDValue const base,
		bool const host,
		bool const initiator
	) noexcept {
		m_value
			= (base & mask_base)
			| (Cmd::IDValue{host} << shift_host)
			| (Cmd::IDValue{initiator} << shift_initiator)
		;
	}

	/**
		Assign to copy with differing initiator.
	*/
	void
	assign(
		ID const& other,
		bool const initiator
	) noexcept {
		m_value
			= (other.m_value & mask_canonical)
			| (Cmd::IDValue{initiator} << shift_initiator)
		;
	}

	/**
		Assign initiator.
	*/
	void
	set_initiator(
		bool const initiator
	) noexcept {
		m_value
			|= (Cmd::IDValue{initiator} << shift_initiator)
		;
	}
/// @}

/** @name Serialization */ /// @{
	static_assert(
		std::is_same<
			Cmd::IDValue,
			std::uint32_t
		>::value, ""
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

static constexpr Cmd::ID const
/**
	Null base %ID value.
*/
ID_NULL{Cmd::IDValue{id_value_null}},
/**
	Maximum base %ID value.
*/
ID_MAX{Cmd::IDValue{id_value_max}};

/**
	Command type info.
*/
struct type_info final {
/** @name Types */ /// @{
	/** Flag store type. */
	using flag_store_type = duct::StateStore<Cmd::Flags, unsigned>;
/// @}

/** @name Properties */ /// @{
	/**
		Command type.
	*/
	Cmd::Type const type;

	/**
		Command flags.
	*/
	flag_store_type const flags;
/// @}

/** @name Operations */ /// @{
	/**
		Construct command.

		@throws std::bad_alloc
		If allocation fails.

		@returns Owning pointer to command.
	*/
	Cmd::UnitUPtr
	(&make)();

	/**
		Construct stage by type.

		@throws Error{ErrorCode::cmd_construct_stage_type_invalid}
		If @a type is not supplied by the command.

		@throws std::bad_alloc
		If allocation fails.

		@returns Owning pointer to stage.
	*/
	Cmd::StageUPtr
	(&make_stage)(
		Cmd::StageType const type
	);
/// @}
};

/**
	Command type info table.

	The type range is <code>[begin, end)</code> such that @c end-begin
	is the number of types in the table in
	<strong>sequential order</strong>.

	@warning Terrors of abyss await you if the table is not
	sequential and/or the range difference is negative.
*/
struct type_info_table final {
/** @name Properties */ /// @{
	// NB: Goddammit, C++. Just let me assign an unbound array
	// reference to an array of any arity.
	/** Type info array. */
	Cmd::type_info const* const* const array;

	/** First type in the array. */
	Cmd::Type const range_begin;

	/** One past the last type in the array. */
	Cmd::Type const range_end;

	/**
		Get size.

		@warning The result will overflow if the range is degenerate.
	*/
	constexpr std::size_t
	size() const noexcept {
		return
			static_cast<std::size_t>(range_end) -
			static_cast<std::size_t>(range_begin)
		;
	}
/// @}

/** @name Queries */ /// @{
	/**
		Check if the table contains a type.
	*/
	constexpr bool
	contains(
		Cmd::Type const type
	) const noexcept {
		return
			type >= range_begin &&
			type <  range_end
		;
	}

	/**
		Check if the type range intersects with another table.
	*/
	constexpr bool
	intersects(
		Cmd::type_info_table const& other
	) const noexcept {
		return
		// end within other
		   ((other.range_end >  this->range_begin &&
			 other.range_end <= this->range_end)  ||
			(this->range_end >  other.range_begin &&
			 this->range_end <= other.range_end))
		// begin within other
		|| ((other.range_begin >= this->range_begin &&
			 other.range_begin <  this->range_end)  ||
			(this->range_begin >= other.range_begin &&
			 this->range_begin <  other.range_end))
		;
	}
/// @}
};

/**
	Command type.

	@remarks The range <code>[0x00, 0xFF]</code> is reserved for stage
	types. The range <code>[0x0100, 0xFFFF]</code> is reserved for 
	standard types. Userspace is permitted the range
	<code>[0x00010000, 0xFFFFFFFF]</code>.

	@sa Cmd::StageType
*/
enum class Type : std::uint32_t {
/** @name Standard range */ /// @{
	/** Base standard type. */
	STANDARD_BASE = 0x100,
	/** Maximum standard type. */
	STANDARD_LIMIT = 0xFFFF,
	/** Number of standard types. */
	STANDARD_COUNT = STANDARD_LIMIT - STANDARD_BASE,
/// @}

/** @name Userspace range */ /// @{
	/** Base userspace type. */
	USERSPACE_BASE = 0x00010000,
	/** Maximum userspace type. */
	USERSPACE_LIMIT = 0xFFFFFFFF,
/// @}

/**
	@ref cmd_generic.
	@{
*/
	GenericTerminate = STANDARD_BASE,
/** @} */

/**
	@ref cmd_hive.
	@{
*/
	HiveInit,
/** @} */

/**
	@ref cmd_node.
	@{
*/
	NodeCreate,
	//NodeDestroy,
/** @} */

/**
	@ref cmd_record.
	@{
*/
	//RecordInsert,
	//RecordDestroy,
/** @} */

/** @cond INTERNAL */
	// One-past-end
	STANDARD_END,
	STANDARD_COUNT_DEFINED = STANDARD_END - STANDARD_BASE
/** @endcond */
};

/** @cond INTERNAL */
static_assert(
	enum_cast(Cmd::Type::STANDARD_LIMIT) >=
	enum_cast(Cmd::Type::STANDARD_END) - 1,
	"standard command types exceed the limit for standard types"
);
/** @endcond */

/**
	Command flags.
*/
enum class Flags : std::uint32_t {
	/**
		No flags.
	*/
	none = 0,

	/**
		Command requires remote result to continue execution.
	*/
	remote = 1 << 1,

	/**
		Command is revertible.

		@note A revision point will only be stored if the command
		has this flag.
	*/
	revertible = 1 << 2
};

/**
	%Stage type.

	@note The range of valid stage types is
	<code>[0x00, 0xFF]</code>, with <code>[0x00, 0x0F]</code> being
	reserved for standard stage types.

	@remarks Most commands use only a subset of the standard stages.
*/
enum class StageType : std::uint8_t {
/** @name Standard range */ /// @{
	/** Base standard type. */
	STANDARD_BASE = 0x00,
	/** Maximum standard type. */
	STANDARD_LIMIT = 0x0F,
/// @}

/** @name Userspace range */ /// @{
	/** Base userspace type. */
	USERSPACE_BASE = 0x10,
	/** Maximum userspace type. */
	USERSPACE_LIMIT = 0xFF,
/// @}

/** @name Standard types */ /// @{
	/**
		Statement stage.

		@remarks If a command uses this stage, it is typically the
		only stage apart from the return stage.
	*/
	Statement = STANDARD_BASE,

	/**
		Error stage.
	*/
	Error,

	/**
		Result stage.

		@remarks Commands needn't use this stage type exclusively for
		command results, but in general it should be used as the final
		result of a request-reply or statement-structured command.
	*/
	Result,

	/**
		Request stage.
	*/
	Request,

	/**
		Response stage.
	*/
	Response,

	/**
		Signal stage.
	*/
	Signal,
/// @}

/** @cond INTERNAL */
	// One-past-end
	STANDARD_END,
	STANDARD_COUNT_DEFINED = STANDARD_END - STANDARD_BASE
/** @endcond */
};

/** @cond INTERNAL */
static_assert(
	enum_cast(Cmd::StageType::STANDARD_LIMIT) >=
	enum_cast(Cmd::StageType::STANDARD_END) - 1,
	"standard stage types exceed the limit for standard types"
);
/** @endcond */

/**
	Command status.
*/
enum class Status : unsigned {
	/**
		Command has completed normally.
	*/
	complete = 1,

	/**
		Command is waiting for a stage to return.
	*/
	waiting,

	/**
		Command has completed with an error.
	*/
	error,

	/**
		Command has completed with a remote error.
	*/
	error_remote,

	/**
		Command has terminated with an unrecoverable error.

		@note Commands which terminate with an error are immediately
		removed from the active group and a @c GenericTerminate
		pseudo-stage is emitted to the remote endpoint if the stage
		isn't a local initiator.

		@par
		@note This is implicit if stage execution throws an exception.
	*/
	fatal,

	/**
		Command has terminated with an unrecoverable remote error.
	*/
	fatal_remote
};

/** @} */ // end of doc-group cmd

} // namespace Cmd
} // namespace Hord

#endif // HORD_CMD_DEFS_HPP_
