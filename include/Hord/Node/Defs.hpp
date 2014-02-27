/**
@file Node/Defs.hpp
@brief Node definitions.

@author Tim Howard
@copyright 2013-2014 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_NODE_DEFS_HPP_
#define HORD_NODE_DEFS_HPP_

#include <Hord/config.hpp>
#include <Hord/Object/Defs.hpp>

namespace Hord {
namespace Node {

// Forward declarations

/**
	@addtogroup object
	@{
*/
/**
	@addtogroup node
	@{
*/

/**
	Node ID.

	@note This type is only to telegraph intent in functions;
	it is entirely equivalent to @c Object::ID.

	@sa Object::ID,
		Rule::ID,
		Hive::ID
*/
using ID = Object::ID;

/**
	Node::ID constants.
*/
enum : Node::ID {
	/**
		Null object.
	*/
	NULL_ID = static_cast<Node::ID>(Object::NULL_ID)
};

/** @} */ // end of doc-group node
/** @} */ // end of doc-group object

} // namespace Node
} // namespace Hord

#endif // HORD_NODE_DEFS_HPP_
