/**
@file Hive/Defs.hpp
@brief Hive definitions.

@author Tim Howard
@copyright 2013-2014 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_HIVE_DEFS_HPP_
#define HORD_HIVE_DEFS_HPP_

#include <Hord/config.hpp>
#include <Hord/Object/Defs.hpp>

namespace Hord {
namespace Hive {

// Forward declarations

/**
	@addtogroup object
	@{
*/
/**
	@addtogroup hive
	@{
*/

/**
	Hive ID.

	@note This type is only to telegraph intent in functions;
	it is entirely equivalent to @c Object::ID.

	@sa Object::ID,
		Rule::ID,
		Node::ID
*/
using ID = Object::ID;

/**
	Hive::ID constants.
*/
enum : Hive::ID {
	/**
		Null object.
	*/
	NULL_ID = static_cast<Hive::ID>(Object::NULL_ID)
};

/** @} */ // end of doc-group hive
/** @} */ // end of doc-group object

} // namespace Hive
} // namespace Hord

#endif // HORD_HIVE_DEFS_HPP_
