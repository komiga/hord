/**
@file
@brief Node unit class.
@ingroup node

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/aux.hpp>
#include <Hord/serialization.hpp>
#include <Hord/Data/Record.hpp>
#include <Hord/IO/PropStream.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Object/Unit.hpp>
#include <Hord/Node/Defs.hpp>
#include <Hord/Node/Column.hpp>

namespace Hord {
namespace Node {

// Forward declarations
class Unit;

/**
	@addtogroup node
	@{
*/

/**
	Base node unit class.

	@note This object supplies the primary and auxiliary props.
	Specialized units shall not change serialization.

	@remarks This is the primary data object in Hord.
*/
class Unit
	: public Object::Unit
{
private:
	using base = Object::Unit;

public:
	/**
		Ensure traits for deriving classes.

		@tparam D Deriving class.
	*/
	template<class D>
	struct ensure_traits
		: base::ensure_traits<D>
	{};

	/** Column vector. */
	using column_vector_type = aux::vector<Node::Column>;

	/** Record vector. */
	using record_vector_type = aux::vector<Data::Record>;

private:
	Node::ID m_layout_ref{Node::ID_NULL};
	column_vector_type m_layout{};
	record_vector_type m_records{};

	Unit() = delete;
	Unit(Unit const&) = delete;
	Unit& operator=(Unit const&) = delete;

	void
	deserialize_prop_primary(
		IO::InputPropStream& prop_stream,
		InputSerializer& ser
	);
	void
	serialize_prop_primary(
		IO::OutputPropStream& prop_stream,
		OutputSerializer& ser
	) const;

	void
	deserialize_prop_auxiliary(
		IO::InputPropStream& prop_stream,
		InputSerializer& ser
	);
	void
	serialize_prop_auxiliary(
		IO::OutputPropStream& prop_stream,
		OutputSerializer& ser
	) const;

// Object::Unit implementation
	void
	deserialize_impl(
		IO::InputPropStream& prop_stream
	) override;

	void
	serialize_impl(
		IO::OutputPropStream& prop_stream
	) const override;

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	virtual
	~Unit() noexcept override = 0;

	/** Move constructor. */
	Unit(Unit&&);
	/** Move assignment operator. */
	Unit& operator=(Unit&&);

protected:
	/**
		Constructor with type information, ID, and parent.

		@post See Object::Unit.
	*/
	Unit(
		Object::type_info const& tinfo,
		Node::ID const id,
		Object::ID const parent
	) noexcept;
/// @}

public:
/** @name Properties */ /// @{
	/**
		Set layout reference.

		@note If @a node_id is non-@c Node::ID_NULL, the layout
		property will be cleared.
	*/
	void
	set_layout_ref(
		Node::ID const node_id
	) noexcept;

	/**
		Get layout reference.
	*/
	Node::ID
	get_layout_ref() const noexcept {
		return m_layout_ref;
	}

	// TODO: Notes for when layout_ref is non-Node::ID_NULL
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
/// @}
};

/** @} */ // end of doc-group node

} // namespace Node
} // namespace Hord
