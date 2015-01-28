/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Command definitions.
@ingroup cmd
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/utility.hpp>

namespace Hord {
namespace Cmd {

// Forward declarations
enum class Result : unsigned;
struct TypeInfo;

/**
	@addtogroup cmd
	@{
*/

/**
	Command ID.
*/
using ID = HashValue;

/**
	Command result.
*/
enum class Result : unsigned {
	/** Success. */
	success = 0,
	/** Success (no action taken). */
	success_no_action,
	/** Error. */
	error,
};

/**
	Command type info.
*/
struct TypeInfo {
/** @name Properties */ /// @{
	/**
		Command ID.
	*/
	Cmd::ID id;

	/**
		Command name.
	*/
	char const* const name;
/// @}
};

/** @} */ // end of doc-group cmd

} // namespace Cmd
} // namespace Hord
