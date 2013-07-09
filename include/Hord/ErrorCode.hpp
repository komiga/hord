/**
@file ErrorCode.hpp
@brief Error codes.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_ERRORCODE_HPP_
#define HORD_ERRORCODE_HPP_

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
	unknown,
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

/** @name Driver */ /// @{
	/**
		Attempted to register a rule type that is reserved for
		standard rules.
	*/
	driver_rule_type_reserved,
	/**
		Attempted to register a rule with permitted_types property
		equal to @c 0.
	*/
	driver_rule_type_zero_permitted_types,
	/**
		Attempted to register a rule type that has already been
		registered.
	*/
	driver_rule_type_shared,

	/**
		Attempted to placehold a hive with an empty root path.
	*/
	driver_hive_root_empty,
	/**
		Attempted to placehold a hive with a root path that is
		shared with another placeheld hive.
	*/
	driver_hive_root_shared,

	/**
		Failed to construct datastore.

		@sa Datastore::type_info
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
		Attempted to request prop from object that does not supply
		it.
	*/
	datastore_prop_unsupplied,
	/**
		Attempted to unlock a prop that is not locked.
	*/
	datastore_prop_not_locked,
/// @}

/** @name Metadata */ /// @{
	/**
		Encountered invalid field type when deserializing.

		@remarks This would indicate malformed prop data.
	*/
	metadata_serialization_type_invalid,
	/**
		Write or read operation failed while deserializing or
		serializing.

		@remarks This will originate from a
		captured @c murk::SerializeError.
	*/
	metadata_serialization_io_failed,
/// @}

/** @name Serialization */ /// @{
	/**
		%Object is not in proper state.

		- When deserializing, this is due to any state other
		  than @c StorageState::placeholder.
		- When serializing, this is due to any state other
		  than @c StorageState::modified (will not be thrown
		  if the serialization was triggered on a hive).
	*/
	serialization_improper_state,
	/**
		Cannot access data.

		When object data cannot be accessed.
	*/
	serialization_access,
/// @}

/** @cond INTERNAL */
	LAST
/** @endcond */
};

/** @} */ // end of doc-group error

} // namespace Hord

#endif // HORD_ERRORCODE_HPP_
