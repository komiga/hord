/**
@file Object.hpp
@brief Object base class.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_OBJECT_HPP_
#define HORD_OBJECT_HPP_

#include "./config.hpp"
#include "./traits.hpp"
#include "./common_types.hpp"
#include "./common_enums.hpp"
#include "./String.hpp"
#include "./Metadata.hpp"

namespace Hord {

// Forward declarations
class Object;

/**
	@addtogroup object
	@{
*/

/**
	Base object.

	@sa Hive,
		Rule,
		Node
*/
class Object {
public:
	/**
		Type info.
	*/
	struct type_info final {
		/**
			%Object type.

			@sa ObjectType
		*/
		ObjectType const type;
	};

	/**
		Ensure traits for deriving classes.

		@tparam D Deriving class.
	*/
	template<typename D>
	struct ensure_traits :
		traits::require_t<
			D,
			tw::capture_post<std::is_base_of, Object>::type,
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
	StorageState m_storage_state{StorageState::null};
	ObjectID m_owner{OBJECT_NULL};
	ObjectID m_id{OBJECT_NULL};
	String m_slug{};
	Metadata m_metadata{};

	Object(Object const&)=delete;
	Object& operator=(Object const&)=delete;

protected:
/** @name Implementation */ /// @{
	/**
		get_type_info() implementation.
	*/
	virtual type_info const&
	get_type_info_impl() const noexcept=0;
/// @}

protected:
/** @name Constructors and destructor */ /// @{
	/**
		Default constructor.

		@post
		@code
			get_storage_state()==StorageState::null &&
			get_owner()==OBJECT_NULL &&
			get_id()==OBJECT_NULL
		@endcode
	*/
	Object();
	/**
		Constructor with storage state, owner, and id.

		@param storage_state Storage state.
		@param owner Owner ID.
		@param id %Object ID.
	*/
	Object(
		StorageState const storage_state,
		ObjectID const owner,
		ObjectID const id
	) noexcept;
	/**
		Constructor with owner and id.

		@param owner Owner ID.
		@param id %Object ID.
	*/
	Object(
		ObjectID const owner,
		ObjectID const id
	) noexcept;
	/** Move constructor. */
	Object(Object&&);
public:
	/** Destructor. */
	virtual
	~Object() noexcept=0;
/// @}

protected:
/** @name Operators */ /// @{
	/** Move assignment operator. */
	Object& operator=(Object&&);
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
	ObjectType
	get_type() const noexcept {
		return get_type_info().type;
	}

	/**
		Set storage state.
		@param storage_state New state.
	*/
	void
	set_storage_state(
		StorageState const storage_state
	) noexcept {
		m_storage_state=storage_state;
	}
	/**
		Get storage state.
		@returns Current storage state.
	*/
	StorageState
	get_storage_state() const noexcept {
		return m_storage_state;
	}

	/**
		Set owner.
		@param owner New owner.
	*/
	void
	set_owner(
		ObjectID const owner
	) noexcept {
		m_owner=owner;
	}
	/**
		Get owner.
		@returns Current owner.
	*/
	ObjectID
	get_owner() const noexcept {
		return m_owner;
	}

	/**
		Set ID.
		@param id New ID.
	*/
	void
	set_id(
		ObjectID const id
	) noexcept {
		m_id=id;
	}
	/**
		Get ID.
		@returns Current ID.
	*/
	ObjectID
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
	Metadata const&
	get_metadata() const noexcept {
		return m_metadata;
	}
	/**
		Get mutable metadata.
		@returns Mutable metadata.
	*/
	Metadata&
	get_metadata() noexcept {
		return m_metadata;
	}
/// @}
};

template struct traits::require_t<
	Object,
	std::has_virtual_destructor
>;

/** @} */ // end of doc-group object

} // namespace Hord

#endif // HORD_OBJECT_HPP_
