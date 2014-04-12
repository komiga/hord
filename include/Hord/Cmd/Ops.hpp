/**
@file Cmd/Ops.hpp
@brief Command operations.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_CMD_OPS_HPP_
#define HORD_CMD_OPS_HPP_

#include <Hord/config.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/Stage.hpp>
#include <Hord/Cmd/Unit.hpp>

#include <iosfwd>

namespace Hord {
namespace Cmd {

// Forward declarations
struct IDPrinter;

/**
	@addtogroup cmd
	@{
*/

/**
	Command %ID printer.
*/
struct IDPrinter {
public:
/** @name Properties */ /// @{
	/**
		Command %ID.
	*/
	Cmd::ID id;
/// @}

private:
	IDPrinter() = delete;

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~IDPrinter() noexcept = default;

	/** Copy constructor. */
	IDPrinter(IDPrinter const&) noexcept = default;
	/** Move constructor. */
	IDPrinter(IDPrinter&&) noexcept = default;

	/**
		Constructor with %ID.

		@param id %ID.
	*/
	constexpr
	IDPrinter(
		Cmd::ID id
	) noexcept
		: id(id)
	{}

	/**
		Constructor with command.

		@post @code
			this->id == cmd.get_id()
		@endcode

		@param cmd Command.
	*/
	IDPrinter(
		Cmd::Unit const& cmd
	) noexcept
		: id(cmd.get_id())
	{}

	/**
		Constructor with command stage.

		@post @code
			this->id == stage.get_id()
		@endcode

		@param stage %Stage.
	*/
	IDPrinter(
		Cmd::Stage const& stage
	) noexcept
		: id(stage.get_id())
	{}
/// @}
};

/**
	Output command %ID to stream.

	@param stream Stream.
	@param printer %ID printer.

	@sa IDPrinter
*/
std::ostream&
operator<<(
	std::ostream& stream,
	Cmd::IDPrinter const& printer
);

/**
	Output command stage identity to stream.

	In the form <code>id[HI]:command_type\@stage_type</code>.

	@param stream Stream.
	@param stage %Stage.
*/
std::ostream&
operator<<(
	std::ostream& stream,
	Cmd::Stage const& stage
);

/** @} */ // end of doc-group cmd

} // namespace Cmd
} // namespace Hord

#endif // HORD_CMD_OPS_HPP_
