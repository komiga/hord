/**
@file Hive/Unit.hpp
@brief Hive class.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_HIVE_UNIT_HPP_
#define HORD_HIVE_UNIT_HPP_

#include <Hord/config.hpp>
#include <Hord/cc_unique_ptr.hpp>
#include <Hord/aux.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Object/Unit.hpp>
#include <Hord/Hive/Defs.hpp>

#include <memory>

namespace Hord {
namespace Hive {

// Forward declarations
class Unit;

/**
	@addtogroup object
	@{
*/
/**
	@addtogroup hive
	@{
*/

/**
	Top-level object container.

	@note This object supplies only the primary data prop.
*/
class Unit final
	: public Object::Unit
{
public:
	/** Object ID set. */
	using id_set_type = aux::unordered_set<Object::ID>;

	/** Object map. */
	using object_map_type
	= aux::unordered_map<
		Object::ID,
		cc_unique_ptr<Object::Unit>
	>;

private:
	using base = Object::Unit;

	id_set_type m_idset{};
	object_map_type m_objects{};

	Unit(Unit const&) = delete;
	Unit& operator=(Unit const&) = delete;

	Object::type_info const&
	get_type_info_impl() const noexcept override;

public:
/** @name Constructors and destructor */ /// @{
	/**
		Default constructor.

		@post
		@code
			get_storage_state() == IO::StorageState::null
		@endcode
	*/
	Unit();

	/**
		Constructor with ID.

		@post See Object::Unit().

		@param id ID.
	*/
	explicit
	Unit(
		Hive::ID const id
	) noexcept;

	/** Move constructor. */
	Unit(Unit&&);
	/** Destructor. */
	~Unit() noexcept override;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Unit& operator=(Unit&&);
/// @}

/** @name Properties */ /// @{
	/**
		Get ID set.
	*/
	id_set_type const&
	get_idset() const noexcept {
		return m_idset;
	}

	/**
		Get mutable ID set.
	*/
	id_set_type&
	get_idset() noexcept {
		return m_idset;
	}

	/**
		Get object map.
	*/
	object_map_type const&
	get_objects() const noexcept {
		return m_objects;
	}

	/**
		Get mutable object map.
	*/
	object_map_type&
	get_objects() noexcept {
		return m_objects;
	}
	/** @} */
/// @}

/** @name Objects */ /// @{
	/**
		Check if a child object exists with the given @a id.

		@param id %Object ID to look for.
	*/
	bool
	has_child(
		Object::ID const id
	) const noexcept(noexcept(m_idset.find(id)));
/// @}
};

/** @} */ // end of doc-group hive
/** @} */ // end of doc-group object

} // namespace Hive

template struct Object::Unit::ensure_traits<Hive::Unit>;

} // namespace Hord

#endif // HORD_HIVE_UNIT_HPP_
