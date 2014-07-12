/**
@file Object/Ops.hpp
@brief Object operations.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Object/Unit.hpp>
#include <Hord/Hive/Unit.hpp>

#include <iosfwd>

namespace Hord {
namespace Object {

// Forward declarations

/**
	@addtogroup object
	@{
*/

/**
	Set object parent.

	@note If <code>object.get_id() == new_parent</code>, the object is
	unmodified and @c false is returned.

	@pre @code
		object.get_base_type() != Object::BaseType::Hive &&
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
	Hive::Unit& hive,
	Object::ID const new_parent
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
