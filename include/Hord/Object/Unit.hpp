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
#include <Hord/IO/Prop.hpp>
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
			std::is_nothrow_destructible,
			tw::is_fully_moveable
		>,
		traits::disallow_t<
			D,
			tw::is_copyable
		>
	{};

private:
	IO::PropStateStore m_prop_states;
	Object::ID m_owner{Object::NULL_ID};
	Object::ID m_id{Object::NULL_ID};
	String m_slug{};
	Data::Metadata m_metadata{};

	Unit() = delete;
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
		Constructor with storage state, owner, and id.

		@post
		@code
			get_storage_state()
			== (get_id() == Object::NULL_ID
				? IO::StorageState::null
				: IO::StorageState::placeholder
			)
		@endcode

		@param supplies_primary Whether IO::PropType::primary is
		supplied by the object.
		@param supplies_auxiliary Whether IO::PropType::auxiliary is
		supplied by the object.
		@param owner Owner ID.
		@param id %Object ID.

		@sa IO::PropStateStore
	*/
	Unit(
		bool const supplies_primary,
		bool const supplies_auxiliary,
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
	*/
	type_info const&
	get_type_info() const noexcept {
		return get_type_info_impl();
	}

	/**
		Get type.
	*/
	Object::Type
	get_type() const noexcept {
		return get_type_info().type;
	}

	/**
		Get storage state.
	*/
	IO::StorageState
	get_storage_state() const noexcept {
		return
			  Object::NULL_ID == m_id
				? IO::StorageState::null
			: m_prop_states.all_original()
				? IO::StorageState::original
			: m_prop_states.any_modified()
				? IO::StorageState::modified
			: IO::StorageState::placeholder
		;
	}

	/**
		Get prop state store.
	*/
	IO::PropStateStore const&
	get_prop_states() const noexcept {
		return m_prop_states;
	}

	/**
		Get mutable prop state store.
	*/
	IO::PropStateStore&
	get_prop_states() noexcept {
		return m_prop_states;
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
	*/
	String const&
	get_slug() const noexcept {
		return m_slug;
	}

	/**
		Get metadata.
	*/
	Data::Metadata const&
	get_metadata() const noexcept {
		return m_metadata;
	}

	/**
		Get mutable metadata.
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
