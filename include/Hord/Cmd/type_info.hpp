/**
@file Cmd/type_info.hpp
@brief Command type information.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_CMD_TYPE_INFO_HPP_
#define HORD_CMD_TYPE_INFO_HPP_

#include <Hord/config.hpp>
#include <Hord/Cmd/Defs.hpp>

namespace Hord {
namespace Cmd {

// Forward declarations

/**
	@addtogroup cmd
	@{
*/

/** @name Type information */ /// @{

/** Cmd::Type::GenericTerminate. */
extern Cmd::type_info const
s_type_info_GenericTerminate;

/** Cmd::Type::NodeCreate. */
extern Cmd::type_info const
s_type_info_NodeCreate;

/// @}

/** @} */ // end of doc-group cmd

} // namespace Cmd
} // namespace Hord

#endif // HORD_CMD_TYPE_INFO_HPP_
