/**
@file String.hpp
@brief String type.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_STRING_HPP_
#define HORD_STRING_HPP_

#include <Hord/config.hpp>
#include <Hord/aux.hpp>

namespace Hord {

/**
	@addtogroup etc
	@{
*/
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

/** @} */ // end of doc-group string
/** @} */ // end of doc-group etc

} // namespace Hord

#endif // HORD_STRING_HPP_
