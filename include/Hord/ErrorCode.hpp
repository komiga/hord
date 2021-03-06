/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief %Error codes.
@ingroup error
*/

#pragma once

#include <Hord/config.hpp>

namespace Hord {

// Forward declarations
enum class ErrorCode : unsigned;

/**
	@addtogroup error
	@{
*/

// FIXME: Doxygen borks all over itself
// when name-groups are used inside enums.

/**
	%Error codes.

	@sa Error
*/
enum class ErrorCode : unsigned {
/** @name General */ /// @{
	/**
		Unknown/unspecified.
	*/
	unknown = 0u,
/// @}

/** @name LockFile */ /// @{
	/**
		Attempted to modify property while lock is active.
	*/
	lockfile_immutable,
	/**
		Failed to acquire the lock.
	*/
	lockfile_acquire_failed,
/// @}

/** @name Table */ /// @{
	/**
		Attempted to configure a table with a column referencing a
		column that did not exist in the original schema.
	*/
	table_column_index_invalid,
	/**
		Attempted to configure a table with a unnamed column.
	*/
	table_column_name_empty,
	/**
		Attempted to configure a table with a non-unique column.
	*/
	table_column_name_shared,
/// @}

/** @name Driver */ /// @{
	/*
		Attempted to register an object unit type that is reserved
		for standard object unit types.
	*/
	// driver_object_type_reserved,
	/**
		Attempted to register an object unit type that has already
		been registered.
	*/
	driver_object_type_shared,

	/**
		Attempted to register a command type info table which
		intersects a type range that is reserved for standard commands
		or stages.
	*/
	driver_command_table_range_invalid,
	/**
		Attempted to register a command type info table which
		intersects the type range of a previously-registered command
		table.
	*/
	driver_command_table_range_shared,

	/**
		Attempted to placehold a datastore with an empty root path.
	*/
	driver_datastore_root_empty,
	/**
		Attempted to placehold a datastore with a root path that is
		shared with another placeheld datastore.
	*/
	driver_datastore_root_shared,
	/**
		Failed to construct datastore.

		@sa IO::Datastore::TypeInfo
	*/
	driver_datastore_construct_failed,
	/**
		Attempted to access datastore that is locked.
	*/
	driver_datastore_locked,
/// @}

/** @name Datastore */ /// @{
	/**
		Attempted to open datastore when it was already open.
	*/
	datastore_open_already,
	/**
		Failed to open datastore.

		Implementation-defined reason.
	*/
	datastore_open_failed,

	/**
		Attempted to perform operation on datastore when it was
		closed.
	*/
	datastore_closed,
	/**
		Attempted to perform locking operation while datastore is
		locked.
	*/
	datastore_locked,
	/**
		Attempted to modify property while datastore was open.

		%Datastore properties are immutable while the datastore is
		open.
	*/
	datastore_property_immutable,

	/**
		Attempted to perform operation with an object not in the
		datastore.
	*/
	datastore_object_not_found,
	/**
		Attempted to create an object that already exists (by ID).
	*/
	datastore_object_already_exists,

	/**
		Attempted to request prop from object that does not supply
		it.
	*/
	datastore_prop_unsupplied,
	/**
		Attempted to request prop from an object that has not yet
		created the prop, or the prop does not exist for some other
		reason (but is otherwise a valid prop).
	*/
	datastore_prop_void,
	/**
		Attempted to unlock a prop that is not locked.
	*/
	datastore_prop_not_locked,
/// @}

/** @name Serialization */ /// @{
	/**
		Object does not supply prop.

		@remarks If the serialization sequence is proper, this
		should not occur from a Unit.
	*/
	serialization_prop_unsupplied,
	/**
		Prop is not in proper state for operation.
	*/
	serialization_prop_improper_state,

	/**
		Write or read operation failed during deserialization or
		serialization.
	*/
	serialization_io_failed,
	/**
		Encountered malformed data during deserialization.
	*/
	serialization_data_malformed,
/// @}

/** @name Command */ /// @{
	/**
		Stage type is not supplied for command.
	*/
	cmd_construct_stage_type_invalid,
/// @}

/** @name Context */ /// @{
	/**
		Datastore does not exist in driver.
	*/
	context_invalid_datastore,

	/**
		Origin stage does not belong to an active command.
	*/
	context_output_detached,
	/**
		Non-initiating stage carries an ID which is not active.
	*/
	context_execute_not_active,
	/**
		Initiating stage carries an ID which is already active.
	*/
	context_execute_already_active,
/// @}

/** @cond INTERNAL */
	LAST
/** @endcond */
};

/** @} */ // end of doc-group error

} // namespace Hord
