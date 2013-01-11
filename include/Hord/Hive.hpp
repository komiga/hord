/**
@file Hive.hpp
@brief Hive class.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_HIVE_HPP_
#define HORD_HIVE_HPP_

#include "./config.hpp"
#include "./common_types.hpp"
#include "./fwd.hpp"
#include "./aux.hpp"
#include "./String.hpp"

#include <utility>
#include <memory>

namespace Hord {

// Forward declarations
class Hive;

/**
	@addtogroup hive
	@{
*/

/**
	Top-level container.
*/
class Hive final {
public:
	/** Node map. */
	typedef aux::unordered_map<NodeID, Node> node_map_type;
	/** Rule map. */
	typedef aux::unordered_map<RuleID, std::unique_ptr<Rule> > rule_map_type;

private:
	typedef aux::unordered_set<ObjectID> object_set_type;

	String m_slug{};
	Metadata m_metadata{};
	object_set_type m_ids{}; // Runtime
	node_map_type m_nodes{};
	rule_map_type m_rules{};

	Hive(Hive const&)=delete;
	Hive& operator=(Hive const&)=delete;
	Hive& operator=(Hive&&)=delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	Hive()=default;
	/** Move constructor. */
	Hive(Hive&&)=default;
	/** Destructor. */
	~Hive()=default;
/// @}

/** @name Properties */ /// @{
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
	inline String const& get_slug() noexcept { return m_slug; }

	/**
		Get metadata.
		@returns The current metadata.
	*/
	inline Metadata const& get_metadata() const noexcept { return m_metadata; }

	/**
		Get node map.
		@returns The current node map.
	*/
	inline node_map_type const& get_nodes() const noexcept { return m_nodes; }
	/**
		Get rule map.
		@returns The current rule map.
	*/
	inline rule_map_type const& get_rules() const noexcept { return m_rules; }
/// @}

/** @name Objects */ /// @{
	/**
		Check if an object exists with the given @a id.
		@returns @c true if @a id is in the hive, or @c false if it is not.
		@param id Object ID to look for.
	*/
	inline bool has_object(ObjectID const id) noexcept(noexcept(m_ids.find(id)))
		{ return m_ids.cend()!=m_ids.find(id); }
	/**
		Check if a Node exists with the given @a id.
		@returns @c true if @a id is in the hive, or @c false if it is not.
		@param id Node ID to look for.
	*/
	inline bool has_node(NodeID const id) noexcept(noexcept(m_nodes.find(id)))
		{ return m_nodes.cend()!=m_nodes.find(id); }
	/**
		Check if a Rule exists with the given @a id.
		@returns @c true if @a id is in the hive, or @c false if it is not.
		@param id Rule ID to look for.
	*/
	inline bool has_rule(RuleID const id) noexcept(noexcept(m_rules.find(id)))
		{ return m_rules.cend()!=m_rules.find(id); }
/// @}
};

/** @} */ // end of doc-group hive

} // namespace Hord

#endif // HORD_HIVE_HPP_
