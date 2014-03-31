/**
@file Cmd/Node.hpp
@brief %Node commands.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_CMD_NODE_HPP_
#define HORD_CMD_NODE_HPP_

#include <Hord/config.hpp>
#include <Hord/String.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Node/Defs.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/type_info.hpp>

namespace Hord {
namespace Cmd {
namespace Node {

/**
	@addtogroup cmd
	@{
*/
/**
	@addtogroup cmd_node
	@{
*/

namespace Create {

/**
	Allocate @c Request stage for the NodeCreate command.

	@throws std::bad_alloc
	If allocation fails.

	@returns Pointer to initiator stage.
	@param parent Parent.
	@param slug %Node slug.
	@param layout_ref Layout reference.
*/
Cmd::StageUPtr
make_request(
	Hord::Object::ID const parent,
	String slug,
	Hord::Node::ID const layout_ref
);

} // namespace Create

/** @} */ // end of doc-group cmd_node
/** @} */ // end of doc-group cmd

} // namespace Node
} // namespace Cmd
} // namespace Hord

#endif // HORD_CMD_NODE_HPP_
