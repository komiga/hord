/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Object operations.
@ingroup object
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/String.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Object/Unit.hpp>
#include <Hord/IO/Datastore.hpp>

#include <iosfwd>

namespace Hord {
namespace Object {

// Forward declarations
struct IDPrinter;

/**
	@addtogroup object
	@{
*/

/**
	Set object parent.

	@note If <code>object.get_id() == new_parent</code>, the object is
	unmodified and @c false is returned.

	@pre @code
		!object.is_null()
	@endcode

	@post If succeeded:
	@code
		object.get_parent() == new_parent
	@endcode

	@post If new_parent does not exist:
	@code
		object.get_parent() == Object::ID_NULL
	@endcode

	@post If state was changed:
	@code
		true == object.get_prop_states().has(
		    IO::PropType::identity,
		    IO::PropState::modified
		)
	@endcode

	@returns @c true if @a object was parented to @a new_parent.
	@c false if attempting to parent an object to itself or to a
	non-existent object.

	@sa Object::Unit::set_parent()
*/
bool
set_parent(
	Object::Unit& object,
	IO::Datastore& datastore,
	Object::ID const new_parent
) noexcept;

/**
	Get the path to an object.
	@{
*/
String
path_to(
	Object::ID const id,
	IO::Datastore const& datastore
) noexcept;

inline String
path_to(
	Object::Unit const& object,
	IO::Datastore const& datastore
) noexcept {
	return path_to(object.get_id(), datastore);
}
/** @} */

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
			this->id == object.get_id()
		@endcode
	*/
	IDPrinter(
		Object::Unit const& object
	) noexcept
		: id(object.get_id())
	{}
/// @}
};

/**
	Output object %ID to stream.

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
*/
std::ostream&
operator<<(
	std::ostream& stream,
	Object::Unit const& object
);

/** @} */ // end of doc-group object

} // namespace Object
} // namespace Hord
