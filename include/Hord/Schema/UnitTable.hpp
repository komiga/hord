/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Table schema unit class.
@ingroup schema
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Schema/Defs.hpp>
#include <Hord/Schema/Unit.hpp>
#include <Hord/Data/TableSchema.hpp>

namespace Hord {
namespace Schema {

// Forward declarations
class UnitTable;

/**
	@addtogroup schema
	@{
*/

/**
	Table schema unit class.
*/
class UnitTable final
	: public Schema::Unit
{
private:
	using base = Schema::Unit;

	static Object::UPtr
	construct(
		Object::ID const id,
		Object::ID const parent
	) noexcept;

public:
	/**
		Type info.
	*/
	static constexpr Object::type_info const
	info{
		"Hord.Schema.Table",
		Schema::Type{Schema::UnitType::Table},
		{true, true},
		UnitTable::construct
	};

private:
	Data::TableSchema m_data_schema{};

	UnitTable() = delete;
	UnitTable(UnitTable const&) = delete;
	UnitTable& operator=(UnitTable const&) = delete;

// Schema::Unit implementation
	void
	deserialize_prop_auxiliary(
		IO::InputPropStream& prop_stream,
		InputSerializer& ser
	) override;

	void
	serialize_prop_auxiliary(
		IO::OutputPropStream& prop_stream,
		OutputSerializer& ser
	) const override;

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~UnitTable() noexcept override;

	/** Move constructor. */
	UnitTable(UnitTable&&);
	/** Move assignment operator. */
	UnitTable& operator=(UnitTable&&);

private:
	UnitTable(
		Schema::ID const id,
		Object::ID const parent
	) noexcept;
/// @}

public:
/** @name Properties */ /// @{
	/**
		Get data schema (mutable).
	*/
	Data::TableSchema&
	get_data_schema() noexcept {
		return m_data_schema;
	}

	/**
		Get data schema.
	*/
	Data::TableSchema const&
	get_data_schema() const noexcept {
		return m_data_schema;
	}
/// @}
};

/** @} */ // end of doc-group schema

} // namespace Schema

template struct Schema::Unit::ensure_traits<Schema::UnitTable>;

} // namespace Hord
