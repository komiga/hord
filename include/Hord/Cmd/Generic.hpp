/**
@file Cmd/Generic.hpp
@brief %Generic commands.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_CMD_GENERIC_HPP_
#define HORD_CMD_GENERIC_HPP_

#include <Hord/config.hpp>
#include <Hord/String.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/type_info.hpp>

namespace Hord {
namespace Cmd {
namespace Generic {

// Forward declarations

/**
	@addtogroup cmd
	@{
*/
/**
	@addtogroup cmd_generic
	@{
*/

/**
	Allocate @c Statement stage for the @c GenericTerminator pseudo-
	command.

	@throws std::bad_alloc
	If allocation fails.

	@returns Pointer to @c Statement stage.
*/
Cmd::StageUPtr
make_terminate();

/** @} */ // end of doc-group cmd_node
/** @} */ // end of doc-group cmd

} // namespace Generic
} // namespace Cmd
} // namespace Hord

#endif // HORD_CMD_GENERIC_HPP_
