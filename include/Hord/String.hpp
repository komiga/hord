/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief String type.
@ingroup string
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/aux.hpp>

#include <cstring>

namespace Hord {

/**
	@addtogroup string
	@{
*/

/**
	Turn input into a UTF-8 encoded string literal.

	@param x_ C-string literal.
*/
#define HORD_STR_LIT(x_) u8 ## x_

/**
	String type.

	@note Contents must be UTF-8.
*/
using String = aux::basic_string<char>;

/**
	Compare strings for equality.
*/
inline bool
string_equal(
	String const& x,
	unsigned const size,
	char const* const data
) {
	return 0 == x.compare(0, x.size(), data, size);
}

/**
	Compare strings for equality.
*/
inline bool
string_equal(
	unsigned const l_size,
	char const* const l_data,
	unsigned const r_size,
	char const* const r_data
) {
	return l_size == r_size && std::strncmp(l_data, r_data, l_size) == 0;
}

/** @} */ // end of doc-group string

} // namespace Hord
