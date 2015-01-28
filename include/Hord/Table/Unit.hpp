/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Table unit class.
@ingroup table
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/aux.hpp>
#include <Hord/serialization.hpp>
#include <Hord/Data/Table.hpp>
#include <Hord/IO/PropStream.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Object/Unit.hpp>
#include <Hord/Schema/Defs.hpp>
#include <Hord/Table/Defs.hpp>

namespace Hord {
namespace Table {

// Forward declarations
class Unit;

/**
	@addtogroup table
	@{
*/

/**
	Base table unit class.

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

private:
	Schema::ID m_schema_ref{Table::ID_NULL};
	Data::Table m_data{};

	Unit() = delete;
	Unit(Unit const&) = delete;
	Unit& operator=(Unit const&) = delete;

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
		Object::TypeInfo const& tinfo,
		Table::ID const id,
		Object::ID const parent
	) noexcept;
/// @}

public:
/** @name Properties */ /// @{
	/**
		Set schema reference.
	*/
	void
	set_schema_ref(
		Schema::ID const schema_id
	) noexcept {
		m_schema_ref = schema_id;
	}

	/**
		Get schema reference.
	*/
	Schema::ID
	schema_ref() const noexcept {
		return m_schema_ref;
	}

	/**
		Get table data (mutable).
	*/
	Data::Table&
	data() noexcept {
		return m_data;
	}

	/**
		Get table data.
	*/
	Data::Table const&
	data() const noexcept {
		return m_data;
	}
/// @}
};

/** @} */ // end of doc-group table

} // namespace Table
} // namespace Hord
