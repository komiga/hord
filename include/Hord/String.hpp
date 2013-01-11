/**
@file String.hpp
@brief String type.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_STRING_HPP_
#define HORD_STRING_HPP_

#include "./config.hpp"
#include "./aux.hpp"

namespace Hord {

/**
	@addtogroup string
	@{
*/

/**
	String literal.
*/
#define HORD_STR_LIT(x) u8 ## x

/**
	String type.
	@note Contents must be UTF-8.
*/
using String=aux::basic_string<char>;

/** @} */ // end of doc-group string

} // namespace Hord

#endif // HORD_STRING_HPP_
