/**
@file config.hpp
@brief Configuration.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_CONFIG_HPP_
#define HORD_CONFIG_HPP_

#include <cstdint>

// Must define allocator before <duct/config.hpp>
#define HORD_STL_ALLOCATOR std::allocator
#define DUCT_CONFIG_STL_ALLOCATOR HORD_STL_ALLOCATOR

#include <duct/config.hpp>

namespace Hord {

} // namespace Hord

#endif // HORD_CONFIG_HPP_
