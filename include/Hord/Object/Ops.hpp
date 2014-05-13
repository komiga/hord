/**
@file Object/Ops.hpp
@brief Object operations.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_OBJECT_OPS_HPP_
#define HORD_OBJECT_OPS_HPP_

#include <Hord/config.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Object/Unit.hpp>

#include <iosfwd>

namespace Hord {
namespace Object {

// Forward declarations

/**
	@addtogroup object
	@{
*/

/**
	Unset object parent.

	@warning The object is not reparented to its hive. It should be
	reparented to an object after calling this to ensure it is not
	orphaned.

	@note The identity property state is unchanged by this function.

	@pre @code
		!object.is_null()
	@endcode

	@post @code
		object.get_parent() == Object::NULL_ID
	@endcode

	@param object Object to unparent.
	@param parent Object's parent.

	@sa Object::Unit::set_parent()
*/
void
unset_parent(
	Object::Unit& object,
	Object::Unit& parent
) noexcept;

/**
	Set object parent.

	@note If @c object.get_id()==new_parent.get_id(), the object is
	unmodified and @c false is returned.

	@par
	@note This should be paired with Object::unset_parent() to ensure
	the object is properly unlinked from its old parent and linked
	to its new parent (especially when its parent is its hive).

	@pre @code
		!object.is_null()
	@endcode

	@post @code
		object.get_parent()
		== (new_parent.get_type() == Object::Type::Hive)
			? Object::NULL_ID
			: new_parent.get_id()
	@endcode

	@post If state was changed:
	@code
		true == object.get_prop_states().has(
		    IO::PropType::identity,
		    IO::PropState::modified
		)
	@endcode

	@returns @c true if @a object was parented to @a new_parent.
	@param object Object to reparent.
	@param new_parent New parent.

	@sa Object::Unit::set_parent()
*/
bool
set_parent(
	Object::Unit& object,
	Object::Unit& new_parent
) noexcept;

/**
	Object %ID printer.
*/
struct IDPrinter {
/** @name Properties */ /// @{
	/**
		Object %ID.
	*/
	Object::ID id;
/// @}

private:
	IDPrinter() = delete;

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~IDPrinter() noexcept = default;

	/** Copy constructor. */
	IDPrinter(IDPrinter const&) noexcept = default;
	/** Move constructor. */
	IDPrinter(IDPrinter&&) noexcept = default;

	/**
		Constructor with object id.

		@param id %ID.
	*/
	constexpr
	IDPrinter(
		Object::ID const id
	) noexcept
		: id(id)
	{}

	/**
		Constructor with object.

		@post @code
			this->id == object.get_id_bare()
		@endcode

		@param object Object.
	*/
	IDPrinter(
		Object::Unit const& object
	) noexcept
		: id(object.get_id_bare())
	{}
/// @}
};

/**
	Output object %ID to stream.

	@param stream Stream.
	@param printer %ID printer.

	@sa IDPrinter
*/
std::ostream&
operator<<(
	std::ostream& stream,
	Object::IDPrinter const& printer
);

/**
	Output object identity to stream.

	In the form <code>id$type\@slug</code>.

	@param stream Stream.
	@param object Object.
*/
std::ostream&
operator<<(
	std::ostream& stream,
	Object::Unit const& object
);

/** @} */ // end of doc-group object

} // namespace Object
} // namespace Hord

#endif // HORD_OBJECT_OPS_HPP_
