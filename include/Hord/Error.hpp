/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief %Error class.
@ingroup error
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/ErrorCode.hpp>
#include <Hord/String.hpp>

#include <duct/GR/Error.hpp>

#include <iosfwd>

namespace Hord {

// Forward declarations

/**
	@addtogroup error
	@{
*/

/**
	%Error class.

	See @c duct::GR::Error.
*/
using Error
= duct::GR::Error<
	Hord::ErrorCode,
	Hord::String
>;

/**
	Get the name of an error.

	@returns C-string containing the name of @a error_code or
	"INVALID" if somehow @a error_code is not actually an ErrorCode.
*/
char const*
get_error_name(
	ErrorCode const error_code
) noexcept;

/**
	Output error to stream.
*/
std::ostream&
operator<<(
	std::ostream& stream,
	Error const& error
);

/** @} */ // end of doc-group error

} // namespace Hord
