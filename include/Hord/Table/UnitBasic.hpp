/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Basic table unit class.
@ingroup table
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Table/Defs.hpp>
#include <Hord/Table/Unit.hpp>

namespace Hord {
namespace Table {

// Forward declarations
class UnitBasic;

/**
	@addtogroup table
	@{
*/

/**
	Basic table unit class.
*/
class UnitBasic final
	: public Table::Unit
{
private:
	using base = Table::Unit;

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
		"Hord.Table.Basic",
		Table::Type{Table::UnitType::Basic},
		{Table::SUPPLIED_PROPS},
		UnitBasic::construct
	};

private:
	UnitBasic() = delete;
	UnitBasic(UnitBasic const&) = delete;
	UnitBasic& operator=(UnitBasic const&) = delete;

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~UnitBasic() noexcept override;

	/** Move constructor. */
	UnitBasic(UnitBasic&&);
	/** Move assignment operator. */
	UnitBasic& operator=(UnitBasic&&);

private:
	UnitBasic(
		Table::ID const id,
		Object::ID const parent
	) noexcept;
/// @}
};

/** @} */ // end of doc-group table

} // namespace Table

template struct Table::Unit::ensure_traits<Table::UnitBasic>;

} // namespace Hord
