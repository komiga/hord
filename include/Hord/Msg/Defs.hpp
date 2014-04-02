/**
@file Msg/Defs.hpp
@brief Messaging definitions.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_MSG_DEFS_HPP_
#define HORD_MSG_DEFS_HPP_

#include <Hord/config.hpp>
#include <Hord/Object/Defs.hpp>

namespace Hord {
namespace Msg {

// Forward declarations

/**
	@addtogroup msg
	@{
*/

/**
	Message ID.

	@sa Msg::Buffer
*/
using MessageID = std::uint32_t;

/** @} */ // end of doc-group msg

} // namespace Msg
} // namespace Hord

#endif // HORD_MSG_DEFS_HPP_
