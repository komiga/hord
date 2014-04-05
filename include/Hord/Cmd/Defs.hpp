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

#include <duct/cc_unique_ptr.hpp>
#include <duct/StateStore.hpp>

namespace Hord {
namespace Cmd {

// Forward declarations
struct IDFields;
struct type_info;
struct type_info_table;
class Stage; // external
enum class Type : std::uint32_t;
enum class Flags : std::uint32_t;
enum class StageType : std::uint8_t;
enum class Status : unsigned;

/**
	@addtogroup cmd
	@{
*/

/**
	%ID.

	@note Value only takes up bits @c 0x3FFFFFFF.
	See @c Cmd::IDFields.
*/
using ID = std::uint32_t;

/**
	%ID constants.
*/
enum : Cmd::ID {
	/**
		Null %ID.
	*/
	NULL_ID = Cmd::ID{0u},
	/**
		Maximum %ID value.
	*/
	MAX_ID = Cmd::ID{0x3FFFFFFFu}
};

/**
	%ID fields.

	@note @c flag_host is included in the canonical value for %ID
	space separation.
*/
struct IDFields final {
/** @name Properties */ /// @{
	/**
		Base %ID value.
	*/
	Cmd::ID base : 30;

	/**
		Whether the command is being executed by the host.
	*/
	bool flag_host : 1;

	/**
		Whether the stage is an initiator for the command.

		@note This is enabled for the first result stage of a
		local command. It ensures there are no stray command
		initiations, and assists when resolving %ID collisions.
	*/
	bool flag_initiator : 1;

	/**
		Assign fields.

		@param base @c base.
		@param flag_host @c flag_host.
		@param flag_initiator @c flag_initiator.
	*/
	void
	assign(
		Cmd::ID const base,
		bool const flag_host,
		bool const flag_initiator
	) noexcept {
		this->base = base;
		this->flag_host = flag_host;
		this->flag_initiator = flag_initiator;
	}

	/**
		Assign.

		@param other Fields to copy.
	*/
	void
	assign(
		IDFields const& other
	) noexcept {
		this->base = other.base;
		this->flag_host = other.flag_host;
		this->flag_initiator = other.flag_initiator;
	}

	/**
		Assign with @c flag_initiator.

		@param other Fields.
		@param flag_initiator @c flag_initiator.
	*/
	void
	assign(
		IDFields const& other,
		bool const flag_initiator
	) noexcept {
		this->base = other.base;
		this->flag_host = other.flag_host;
		this->flag_initiator = flag_initiator;
	}
/// @}
};

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
		Construct stage by type.

		@throws Error{ErrorCode::cmd_construct_stage_type_invalid}
		If @a type is not supplied by the command.

		@throws std::bad_alloc
		If allocation fails.

		@returns Pointer to stage.
		@param type %Stage type.
	*/
	Cmd::Stage*
	(&construct_stage)(
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

		@param type Command type.
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

		@param other Table to test against.
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
	Owning pointer to command stage.
*/
using StageUPtr = duct::cc_unique_ptr<Cmd::Stage>;

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
		Command has completed without explicit error.

		@note This will be the command status even if an Error stage
		is emitted.
	*/
	complete = 1,

	/**
		Command is waiting for a stage to return.
	*/
	waiting,

	/**
		Command has terminated with an error.

		@note Commands which terminate with an error are immediately
		removed from the active group and a @c GenericTerminate
		pseudo-stage is emitted to the remote endpoint if the stage
		isn't a local initiator.

		@par
		@note This is implicit if stage execution throws an exception.
	*/
	error,

	/**
		Command has terminated because of a remote error.
	*/
	error_remote
};

/** @} */ // end of doc-group cmd

} // namespace Cmd
} // namespace Hord

#endif // HORD_CMD_DEFS_HPP_
