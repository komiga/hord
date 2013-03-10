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
#include "./String.hpp"
#include "./Metadata.hpp"
#include "./Object.hpp"
#include "./Record.hpp"
#include "./Column.hpp"

namespace Hord {

// Forward declarations
class Node;

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
	friend class Driver;
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
	~Node() override=default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Node& operator=(Node&&)=default;
/// @}
};

/** @} */ // end of doc-group node

} // namespace Hord

#endif // HORD_NODE_HPP_
