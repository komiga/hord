/**
@file Object/Ops.hpp
@brief Object operations.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_OBJECT_OPS_HPP_
#define HORD_OBJECT_OPS_HPP_

#include <Hord/config.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Object/Unit.hpp>

namespace Hord {
namespace Object {

// Forward declarations

/**
	@addtogroup object
	@{
*/

/**
	Unset object parent.

	@pre @code
		object.is_identified()
	@endcode

	@post @code
		object.get_parent() == Object::NULL_ID
	@endcode

	@param object Object to unparent.
	@param parent Object's parent.
*/
void
unset_parent(
	Object::Unit& object,
	Object::Unit& parent
) noexcept;

/**
	Set object parent.

	@pre @code
		object.is_identified()
	@end

	@post @code
		object.get_parent()
		== (new_parent.get_type() == Object::Type::Hive)
			? Object::NULL_ID
			: new_parent.get_id()
	@endcode

	@note If @c object.get_id()==new_parent.get_id(), the object is
	unmodified and @c false is returned.

	@par
	@note This should be paired with Object::unset_parent() to ensure
	the object is properly unlinked from its old parent and linked
	to its new parent (especially when its parent is its hive).

	@returns @c true if @a object was parented to @a new_parent.
	@param object Object to reparent.
	@param new_parent New parent.
*/
bool
set_parent(
	Object::Unit& object,
	Object::Unit& new_parent
) noexcept;

/** @} */ // end of doc-group object

} // namespace Object
} // namespace Hord

#endif // HORD_OBJECT_OPS_HPP_
