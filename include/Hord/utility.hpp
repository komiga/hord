/**
@file utility.hpp
@brief Utilities.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_UTILITY_HPP_
#define HORD_UTILITY_HPP_

#include "./config.hpp"

#include <type_traits>

namespace Hord {

// Forward declarations

/**
	@addtogroup etc
	@{
*/
/**
	@addtogroup utility
	@{
*/

/**
	Make a reference to value a reference to const value.

	@note @c const_cast without the template parameters.

	@param value Value.
*/
template<
	typename T
>
inline typename std::add_const<T>::type const&
make_const(
	T& value
) noexcept {
	return value;
}

/** @} */ // end of doc-group utility
/** @} */ // end of doc-group etc

} // namespace Hord

#endif // HORD_UTILITY_HPP_
