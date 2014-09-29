/**
@file Cmd/Defs.hpp
@brief Command definitions.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>

#include <am/hash/common.hpp>
#include <am/hash/murmur.hpp>

namespace Hord {
namespace Cmd {

// Forward declarations
enum class Result : unsigned;
struct type_info;

/**
	@addtogroup cmd
	@{
*/

/** @cond INTERNAL */
static constexpr am::hash::HashLength const
ID_HASH_LENGTH{am::hash::HL32};
/** @endcond */ // INTERNAL

/**
	Command ID.
*/
using ID = am::detail::hash::murmur_hash_type<Cmd::ID_HASH_LENGTH>;

/** @cond INTERNAL */
enum : Cmd::ID {
	ID_HASH_SEED = Cmd::ID{0xa57634c5}
};
/** @endcond */ // INTERNAL

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
struct type_info {
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
