/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief %Table commands.
@ingroup cmd_table
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Schema/Defs.hpp>
#include <Hord/Table/Defs.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/Unit.hpp>

namespace Hord {
namespace Cmd {
namespace Table {

// Forward declarations
class Create;

/**
	@addtogroup cmd_table
	@{
*/

/**
	Table create command.
*/
class Create final
	: public Cmd::Unit<Create>
{
	HORD_CMD_IMPL_BOILERPLATE_WITH_COMMIT(
		Create,
		"Cmd::Table::Create"
	);

public:
/** @name Operations */ /// @{
	/**
		Create table.

		If @a attach_to_schema is @c true, the table's schema_ref
		property will be set to @a schema_id.

		If @a schema_id is non-null, it must refer to a @ref schema.
	*/
	exec_result_type
	operator()(
		Hord::Object::ID parent_id,
		Hord::Schema::ID schema_id,
		String const& slug,
		Hord::Table::UnitType unit_type,
		bool attach_to_schema
	) noexcept;
/// @}
};

/** @} */ // end of doc-group cmd_table

} // namespace Table

/** @cond INTERNAL */
HORD_CMD_IMPL_ENSURE_TRAITS(Cmd::Table::Create);
/** @endcond */ // INTERNAL

} // namespace Cmd
} // namespace Hord
