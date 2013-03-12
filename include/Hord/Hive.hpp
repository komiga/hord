/**
@file Hive.hpp
@brief Hive class.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_HIVE_HPP_
#define HORD_HIVE_HPP_

#include "./config.hpp"
#include "./common_types.hpp"
#include "./common_enums.hpp"
#include "./aux.hpp"
#include "./String.hpp"
#include "./Object.hpp"

#include <memory>

namespace Hord {

// Forward declarations
class Hive;

/**
	@addtogroup driver
	@{
*/
/**
	@addtogroup hive
	@{
*/

/**
	Top-level Object container.
*/
class Hive final
	: public Object {
public:
	friend class Driver;

	/** Object ID set. */
	typedef aux::unordered_set<ObjectID> id_set_type;
	/** Object map. */
	typedef aux::unordered_map<
		ObjectID, std::unique_ptr<Object>
	> object_map_type;

private:
	String m_root{};
	id_set_type m_idset{};
	object_map_type m_objects{};

	Hive(Hive const&)=delete;
	Hive& operator=(Hive const&)=delete;

	Object::type_info const& get_type_info_impl() const noexcept override;

public:
/** @name Constructors and destructor */ /// @{
	/**
		Default constructor.

		@post See Object::Object().
	*/
	Hive()=default;
	/**
		Constructor with root path.

		@post
		@code
			get_storage_state()
			==(get_root().empty()
				? StorageState::null
				: StorageState::placeholder
			)
		@endcode
		@param id ID.
		@param root Root path.
	*/
	explicit Hive(ObjectID id, String root) noexcept
		: Object{
			root.empty()
				? StorageState::null
				: StorageState::placeholder
			, OBJECT_NULL
			, id
		}
		, m_root{std::move(root)}
	{}
	/** Move constructor. */
	Hive(Hive&&)=default;
	/** Destructor. */
	~Hive() override=default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Hive& operator=(Hive&&)=default;
/// @}

/** @name Properties */ /// @{
	/**
		Set root path.

		@throws Error{ErrorCode::mutate_hive_root_empty}
		If @a root is empty. It is illegal to clear the root path
		after a Hive has been constructed.

		@param root New root path.
	*/
	void set_root(String root);
	/**
		Get root path.
		@returns Current root path.
	*/
	String const& get_root() const noexcept
		{ return m_root; }

	/**
		Get ID set.
		@returns Current ID set.
	*/
	id_set_type const& get_idset() const noexcept
		{ return m_idset; }
	/**
		Get mutable ID set.
		@returns Mutable ID set.
	*/
	id_set_type& get_idset() noexcept
		{ return m_idset; }

	/**
		Get object map.
		@returns Current object map.
	*/
	object_map_type const& get_objects() const noexcept
		{ return m_objects; }
	/**
		Get mutable object map.
		@returns Mutable object map.
	*/
	object_map_type& get_objects() noexcept
		{ return m_objects; }
/// @}

/** @name Objects */ /// @{
	/**
		Check if a child Object exists with the given @a id.

		@returns
		- @c true if @a id is in the hive, or
		- @c false if it is not.
		@param id Object ID to look for.
	*/
	bool has_child(
		ObjectID const id
	) const noexcept(noexcept(m_idset.find(id)));
/// @}
};

/** @} */ // end of doc-group hive
/** @} */ // end of doc-group driver

} // namespace Hord

#endif // HORD_HIVE_HPP_
