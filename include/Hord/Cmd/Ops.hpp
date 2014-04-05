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

#include <iosfwd>

namespace Hord {
namespace Cmd {

// Forward declarations
struct IDFieldsPrinter;

/**
	@addtogroup cmd
	@{
*/

/**
	Command %ID fields printer.
*/
struct IDFieldsPrinter {
/** @name Properties */ /// @{
	/**
		Command %ID fields.
	*/
	Cmd::IDFields fields;
/// @}

private:
	IDFieldsPrinter() = delete;

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~IDFieldsPrinter() noexcept = default;

	/** Copy constructor. */
	IDFieldsPrinter(IDFieldsPrinter const&) noexcept = default;
	/** Move constructor. */
	IDFieldsPrinter(IDFieldsPrinter&&) noexcept = default;

	/**
		Constructor with %ID fields.

		@param fields Fields.
	*/
	constexpr
	IDFieldsPrinter(
		Cmd::IDFields fields
	) noexcept
		: fields(fields)
	{}

	/**
		Constructor with stage.

		@post @code
			this->fields == stage.get_id_fields()
		@endcode

		@param stage %Stage.
	*/
	IDFieldsPrinter(
		Cmd::Stage const& stage
	) noexcept
		: fields(stage.get_id_fields())
	{}
/// @}
};

/**
	Output command %ID fields to stream.

	@param stream Stream.
	@param printer %ID printer.

	@sa IDFieldsPrinter
*/
std::ostream&
operator<<(
	std::ostream& stream,
	Cmd::IDFieldsPrinter const& printer
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
