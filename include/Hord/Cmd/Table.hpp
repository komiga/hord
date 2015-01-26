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
		Execute command.

		@note IO::PropType::identity is implicit, and any types not
		supplied for a particular object are ignored when loading
		props.
	*/
	exec_result_type
	operator()(
		Hord::Object::ID const parent,
		Hord::Table::ID const schema_ref,
		String const& slug,
		Hord::Table::UnitType const unit_type
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
