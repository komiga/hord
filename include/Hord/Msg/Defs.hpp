/**
@file Msg/Defs.hpp
@brief Messaging definitions.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
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
using MessageID = uint32_t;

/** @} */ // end of doc-group msg

} // namespace Msg
} // namespace Hord

#endif // HORD_MSG_DEFS_HPP_
