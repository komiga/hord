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
	Object base class.
*/
class Object {
public:
	friend class Driver;

	/**
		Type info.
	*/
	struct type_info final {
		/**
			Object type.

			@sa ObjectType
		*/
		ObjectType const type;
	};

private:
	StorageState m_storage_state{StorageState::null};
	ObjectID m_owner{OBJECT_NULL};
	ObjectID m_id{OBJECT_NULL};
	String m_slug{};
	Metadata m_metadata{};

	Object(Object const&)=delete;
	Object& operator=(Object const&)=delete;

	/**
		get_type_info() implementation.
		@returns The object's type info.
	*/
	virtual type_info const& get_type_info_impl() const noexcept=0;

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
	Object()=default;
	/**
		Constructor with storage state, owner, and id.

		@param storage_state Storage state.
		@param owner Owner ID.
		@param id Object ID.
	*/
	Object(
		StorageState storage_state,
		ObjectID owner,
		ObjectID id
	) noexcept
		: m_storage_state{storage_state}
		, m_owner{owner}
		, m_id{id}
	{}
	/**
		Constructor with owner and id.

		@param owner Owner ID.
		@param id Object ID.
	*/
	Object(ObjectID owner, ObjectID id) noexcept
		: m_owner{owner}
		, m_id{id}
	{}
	/** Move constructor. */
	Object(Object&&)=default;
public:
	/** Destructor. */
	inline virtual ~Object()=0;

protected:
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Object& operator=(Object&&)=default;
/// @}

public:
/** @name Properties */ /// @{
	/**
		Get type info.
		@returns The object's type info.
	*/
	type_info const& get_type_info() const noexcept
		{ return get_type_info_impl(); }

	/**
		Get storage state.
		@returns Current storage state.
	*/
	StorageState get_storage_state() const noexcept
		{ return m_storage_state; }

	/**
		Get owner.
		@returns Current owner.
	*/
	ObjectID get_owner() const noexcept
		{ return m_owner; }

	/**
		Get ID.
		@returns Current ID.
	*/
	ObjectID get_id() const noexcept
		{ return m_id; }

	/**
		Get slug.
		@returns Current slug.
	*/
	String const& get_slug() const noexcept
		{ return m_slug; }

	/**
		Get metadata.
		@returns Current metadata.
	*/
	Metadata const& get_metadata() const noexcept
		{ return m_metadata; }
/// @}
};
inline Object::~Object()=default;

/** @} */ // end of doc-group object

} // namespace Hord

#endif // HORD_OBJECT_HPP_