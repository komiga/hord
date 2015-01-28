/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Table operations.
@ingroup table
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/IO/Datastore.hpp>
#include <Hord/Schema/Defs.hpp>
#include <Hord/Table/Defs.hpp>
#include <Hord/Table/Unit.hpp>

namespace Hord {
namespace Table {

// Forward declarations

/**
	@addtogroup table
	@{
*/

/**
	Set schema reference.
*/
bool
set_schema_ref(
	Table::Unit& table,
	IO::Datastore& datastore,
	Schema::ID new_schema_ref
);

/** @} */ // end of doc-group table

} // namespace Table
} // namespace Hord
