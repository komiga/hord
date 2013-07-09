/**
@file Object/Unit.hpp
@brief Object base class.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_OBJECT_UNIT_HPP_
#define HORD_OBJECT_UNIT_HPP_

#include <Hord/config.hpp>
#include <Hord/traits.hpp>
#include <Hord/String.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/Data/Metadata.hpp>
#include <Hord/Object/Defs.hpp>

namespace Hord {
namespace Object {

// Forward declarations
class Unit;

/**
	@addtogroup object
	@{
*/

/**
	Base object.

	@sa Hive::Unit,
		Rule::Unit,
		Node::Unit
*/
class Unit {
public:
	/** Type info. */
	using type_info = Object::type_info;

	/**
		Ensure traits for deriving classes.

		@tparam D Deriving class.
	*/
	template<
		typename D
	>
	struct ensure_traits :
		traits::require_t<
			D,
			tw::capture_post<std::is_base_of, Object::Unit>::type,
			// FIXME: libstdc++ 4.7.3 doesn't have the nothrow version
			std::is_destructible,
			tw::is_fully_moveable
		>,
		traits::disallow_t<
			D,
			tw::is_copyable
		>
	{};

private:
	IO::StorageState m_storage_state{IO::StorageState::null};
	Object::ID m_owner{Object::NULL_ID};
	Object::ID m_id{Object::NULL_ID};
	String m_slug{};
	Data::Metadata m_metadata{};

	Unit(Unit const&) = delete;
	Unit& operator=(Unit const&) = delete;

protected:
/** @name Implementation */ /// @{
	/**
		get_type_info() implementation.
	*/
	virtual type_info const&
	get_type_info_impl() const noexcept = 0;
/// @}

protected:
/** @name Constructors and destructor */ /// @{
	/**
		Default constructor.

		@post
		@code
			get_storage_state() == IO::StorageState::null &&
			get_owner() == Object::NULL_ID &&
			get_id() == Object::NULL_ID
		@endcode
	*/
	Unit();

	/**
		Constructor with storage state, owner, and id.

		@param storage_state Storage state.
		@param owner Owner ID.
		@param id %Object ID.
	*/
	Unit(
		IO::StorageState const storage_state,
		Object::ID const owner,
		Object::ID const id
	) noexcept;

	/** Move constructor. */
	Unit(Unit&&);
public:
	/** Destructor. */
	virtual
	~Unit() noexcept = 0;
/// @}

protected:
/** @name Operators */ /// @{
	/** Move assignment operator. */
	Unit& operator=(Unit&&);
/// @}

public:
/** @name Properties */ /// @{
	/**
		Get type info.

		@returns The object's type info.
	*/
	type_info const&
	get_type_info() const noexcept {
		return get_type_info_impl();
	}

	/**
		Get type.

		@returns Type.
	*/
	Object::Type
	get_type() const noexcept {
		return get_type_info().type;
	}

	/**
		Set storage state.

		@param storage_state New state.
	*/
	void
	set_storage_state(
		IO::StorageState const storage_state
	) noexcept {
		m_storage_state = storage_state;
	}

	/**
		Get storage state.

		@returns Current storage state.
	*/
	IO::StorageState
	get_storage_state() const noexcept {
		return m_storage_state;
	}

	/**
		Set owner.

		@param owner New owner.
	*/
	void
	set_owner(
		Object::ID const owner
	) noexcept {
		m_owner = owner;
	}

	/**
		Get owner.

		@returns Current owner.
	*/
	Object::ID
	get_owner() const noexcept {
		return m_owner;
	}

	/**
		Set ID.

		@param id New ID.
	*/
	void
	set_id(
		Object::ID const id
	) noexcept {
		m_id = id;
	}

	/**
		Get ID.

		@returns Current ID.
	*/
	Object::ID
	get_id() const noexcept {
		return m_id;
	}

	/**
		Set slug.

		@warning This property is truncated to 64 code units.
		@param slug New slug.
	*/
	void
	set_slug(
		String slug
	) noexcept;

	/**
		Get slug.

		@returns Current slug.
	*/
	String const&
	get_slug() const noexcept {
		return m_slug;
	}

	/**
		Get metadata.

		@returns Current metadata.
	*/
	Data::Metadata const&
	get_metadata() const noexcept {
		return m_metadata;
	}

	/**
		Get mutable metadata.

		@returns Mutable metadata.
	*/
	Data::Metadata&
	get_metadata() noexcept {
		return m_metadata;
	}
/// @}
};

/** @} */ // end of doc-group object

} // namespace Object

template struct traits::require_t<
	Object::Unit,
	std::has_virtual_destructor
>;

} // namespace Hord

#endif // HORD_OBJECT_UNIT_HPP_
