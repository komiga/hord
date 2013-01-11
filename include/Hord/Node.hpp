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
#include "./fwd.hpp"
#include "./aux.hpp"
#include "./String.hpp"

#include <functional>

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
class Node final {
public:
	/** Column vector. */
	typedef aux::vector<Column> column_vector_type;
	/** Record vector. */
	typedef aux::vector<Record> record_vector_type;
	/** Child vector. */
	typedef aux::vector<NodeID> child_vector_type;

private:
	std::reference_wrapper<Hive> m_owner; // Runtime
	NodeID m_id;
	String m_slug{};
	Metadata m_metadata{};
	NodeID m_layout_ref{0};
	column_vector_type m_layout{};
	record_vector_type m_records{};
	child_vector_type m_children{};

	Node()=delete;
	Node(Node const&)=delete;
	Node& operator=(Node const&)=delete;
	Node& operator=(Node&&)=delete;

public:
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with owner and ID.
		@param owner Owner.
		@param id ID.
	*/
	Node(Hive& owner, NodeID const id)
		: m_owner{owner}
		, m_id{id}
	{}
	/** Move constructor. */
	Node(Node&&)=default;
	/** Destructor. */
	~Node()=default;
/// @}

/** @name Properties */ /// @{
	/**
		Get ID.
		@returns Current ID.
	*/
	inline NodeID get_id() const noexcept { return m_id; }

	/**
		Assign slug.
		@warning New slug will be truncated to 64 code units.
		@param slug New slug.
	*/
	inline void set_slug(String slug) { m_slug.assign(slug, 0, 64); }
	/**
		Get slug.
		@returns Current slug.
	*/
	inline String const& get_slug() const noexcept { return m_slug; }

	/**
		Get metadata.
		@returns The current metadata.
	*/
	inline Metadata const& get_metadata() const noexcept { return m_metadata; }
/// @}
};

/** @} */ // end of doc-group node

} // namespace Hord

#endif // HORD_NODE_HPP_
