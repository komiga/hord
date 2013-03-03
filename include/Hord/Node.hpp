/**
@file Node.hpp
@brief Node class.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_NODE_HPP_
#define HORD_NODE_HPP_

#include "./config.hpp"
#include "./common_types.hpp"
#include "./aux.hpp"
#include "./String.hpp"
#include "./Metadata.hpp"
#include "./Record.hpp"
#include "./Rule.hpp"
#include "./Column.hpp"

#include <functional>

namespace Hord {

// Forward declarations
class Node;
class Hive; // ext

/**
	@addtogroup node
	@{
*/

/**
	Node.
*/
class Node final {
public:
	friend class Driver;
	/** Column vector. */
	typedef aux::vector<Column> column_vector_type;
	/** Record vector. */
	typedef aux::vector<Record> record_vector_type;
	/** Child vector. */
	typedef aux::vector<NodeID> child_vector_type;

private:
	mutable StorageState m_storage_state{StorageState::null}; // Runtime
	std::reference_wrapper<Hive> m_owner; // Runtime
	NodeID m_id{OBJECT_NULL};
	String m_slug{};
	Metadata m_metadata{};
	NodeID m_layout_ref{OBJECT_NULL};
	column_vector_type m_layout{};
	record_vector_type m_records{};
	child_vector_type m_children{};

	Node()=delete;
	Node(Node const&)=delete;
	Node& operator=(Node const&)=delete;

public:
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with owner and ID.
		@post @code get_storage_state()==(OBJECT_NULL==get_id() ? StorageState::null : StorageState::placeholder) @endcode
		@param owner Owner.
		@param id ID.
	*/
	Node(Hive& owner, NodeID id)
		: m_storage_state{(OBJECT_NULL==id) ? StorageState::null : StorageState::placeholder}
		, m_owner{owner}
		, m_id{id}
	{}
	/** Move constructor. */
	Node(Node&&)=default;
	/** Destructor. */
	~Node()=default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Node& operator=(Node&&)=default;
/// @}

/** @name Properties */ /// @{
	/**
		Get storage state.
		@returns Current storage state.
	*/
	StorageState get_storage_state() const noexcept { return m_storage_state; }

	/**
		Get owner.
		@returns Current owner.
	*/
	Hive& get_owner() const noexcept { return m_owner.get(); }

	/**
		Get ID.
		@returns Current ID.
	*/
	NodeID get_id() const noexcept { return m_id; }

	/**
		Get slug.
		@returns Current slug.
	*/
	String const& get_slug() const noexcept { return m_slug; }

	/**
		Get metadata.
		@returns The current metadata.
	*/
	Metadata const& get_metadata() const noexcept { return m_metadata; }
/// @}
};

/** @} */ // end of doc-group node

} // namespace Hord

#endif // HORD_NODE_HPP_
