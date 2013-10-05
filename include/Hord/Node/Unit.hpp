/**
@file Node/Unit.hpp
@brief Node unit class.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_NODE_UNIT_HPP_
#define HORD_NODE_UNIT_HPP_

#include <Hord/config.hpp>
#include <Hord/aux.hpp>
#include <Hord/Data/Record.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Object/Unit.hpp>
#include <Hord/Hive/Defs.hpp>
#include <Hord/Node/Defs.hpp>
#include <Hord/Node/Column.hpp>

namespace Hord {
namespace Node {

// Forward declarations
class Unit;

/**
	@addtogroup object
	@{
*/
/**
	@addtogroup node
	@{
*/

/**
	Node unit class.
*/
class Unit final
	: public Object::Unit
{
public:
	/** Base class. */
	using base = Object::Unit;

	/** Column vector. */
	using column_vector_type = aux::vector<Node::Column>;
	/** Record vector. */
	using record_vector_type = aux::vector<Data::Record>;
	/** Child vector. */
	using child_vector_type = aux::vector<Node::ID>;

private:
	Node::ID m_layout_ref{static_cast<Node::ID>(Object::NULL_ID)};
	column_vector_type m_layout{};
	record_vector_type m_records{};
	child_vector_type m_children{};

	Unit() = delete;
	Unit(Unit const&) = delete;
	Unit& operator=(Unit const&) = delete;

	Object::type_info const&
	get_type_info_impl() const noexcept override;

public:
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with owner and ID.

		@post See Object::Unit().

		@param owner Owner.
		@param id ID.
	*/
	Unit(
		Hive::ID const owner,
		Node::ID const id
	) noexcept;

	/** Move constructor. */
	Unit(Unit&&);
	/** Destructor. */
	~Unit() noexcept override;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Unit& operator=(Unit&&);
/// @}

/** @name Properties */ /// @{
	/**
		Set layout reference.

		@note If @a node_id is non-@c Object::NULL_ID, the layout
		property will be cleared.
		@param node_id New layout reference.
	*/
	void
	set_layout_ref(
		Node::ID const node_id
	) noexcept;

	/**
		Get layout reference.
	*/
	Object::ID
	get_layout_ref() const noexcept {
		return m_layout_ref;
	}

	// TODO: Notes for when layout_ref is non-Object::NULL_ID
	/**
		Get layout.
	*/
	column_vector_type const&
	get_layout() const noexcept {
		return m_layout;
	}

	/**
		Get mutable layout.
	*/
	column_vector_type&
	get_layout() noexcept {
		return m_layout;
	}

	/**
		Get record collection.
	*/
	record_vector_type const&
	get_records() const noexcept {
		return m_records;
	}

	/**
		Get mutable record collection.
	*/
	record_vector_type&
	get_records() noexcept {
		return m_records;
	}

	/**
		Get child collection.
	*/
	child_vector_type const&
	get_children() const noexcept {
		return m_children;
	}

	/**
		Get mutable child collection.
	*/
	child_vector_type&
	get_children() noexcept {
		return m_children;
	}
/// @}
};

/** @} */ // end of doc-group node
/** @} */ // end of doc-group object

} // namespace Node

template struct Object::Unit::ensure_traits<Node::Unit>;

} // namespace Hord

#endif // HORD_NODE_UNIT_HPP_
