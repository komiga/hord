/**
@file Node.hpp
@brief Node class.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_NODE_HPP_
#define HORD_NODE_HPP_

#include "./config.hpp"
#include "./common_types.hpp"
#include "./common_enums.hpp"
#include "./aux.hpp"
#include "./Object.hpp"
#include "./Record.hpp"
#include "./Column.hpp"

namespace Hord {

// Forward declarations
class Node;

/**
	@addtogroup object
	@{
*/
/**
	@addtogroup node
	@{
*/

/**
	Node.
*/
class Node final
	: public Object {
public:
	/** Column vector. */
	typedef aux::vector<Column> column_vector_type;
	/** Record vector. */
	typedef aux::vector<Record> record_vector_type;
	/** Child vector. */
	typedef aux::vector<NodeID> child_vector_type;

private:
	NodeID m_layout_ref{OBJECT_NULL};
	column_vector_type m_layout{};
	record_vector_type m_records{};
	child_vector_type m_children{};

	Node()=delete;
	Node(Node const&)=delete;
	Node& operator=(Node const&)=delete;

	Object::type_info const& get_type_info_impl() const noexcept override;

public:
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with owner and ID.

		@post
		@code
			get_storage_state()
			==(OBJECT_NULL==get_id()
				? StorageState::null
				: StorageState::placeholder
			)
		@endcode

		@param owner Owner.
		@param id ID.
	*/
	Node(HiveID owner, NodeID id) noexcept
		: Object{
			(OBJECT_NULL==id)
				? StorageState::null
				: StorageState::placeholder
			, ObjectID{owner}
			, ObjectID{id}
		}
	{}
	/** Move constructor. */
	Node(Node&&)=default;
	/** Destructor. */
	~Node() override;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Node& operator=(Node&&)=default;
/// @}

/** @name Properties */ /// @{
	/**
		Set layout reference.

		@note If @a node_id is non-@c OBJECT_NULL, the layout property
		will be cleared.
		@param node_id New layout reference.
	*/
	void set_layout_ref(NodeID const node_id) noexcept;
	/**
		Get layout reference.
		@returns Current layout reference.
	*/
	ObjectID get_layout_ref() const noexcept
		{ return m_layout_ref; }

	// TODO: Notes for when layout_ref is non-OBJECT_NULL
	/**
		Get layout.
		@returns Current layout.
	*/
	column_vector_type const& get_layout() const noexcept
		{ return m_layout; }
	/**
		Get mutable layout.
		@returns Mutable layout.
	*/
	column_vector_type& get_layout() noexcept
		{ return m_layout; }

	/**
		Get record collection.
		@returns Current record collection.
	*/
	record_vector_type const& get_records() const noexcept
		{ return m_records; }
	/**
		Get mutable record collection.
		@returns Mutable record collection.
	*/
	record_vector_type& get_records() noexcept
		{ return m_records; }

	/**
		Get child collection.
		@returns Current child collection.
	*/
	child_vector_type const& get_children() const noexcept
		{ return m_children; }
	/**
		Get mutable child collection.
		@returns Mutable child collection.
	*/
	child_vector_type& get_children() noexcept
		{ return m_children; }
/// @}
};

/** @} */ // end of doc-group node
/** @} */ // end of doc-group object

} // namespace Hord

#endif // HORD_NODE_HPP_
