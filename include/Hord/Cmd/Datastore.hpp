/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief %Datastore commands.
@ingroup cmd_datastore
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/Unit.hpp>

namespace Hord {
namespace Cmd {
namespace Datastore {

// Forward declarations
class Init;
class Store;

/**
	@addtogroup cmd_datastore
	@{
*/

/**
	Datastore init command.
*/
class Init final
	: public Cmd::Unit<Init>
{
	HORD_CMD_IMPL_BOILERPLATE(
		Init,
		"Cmd::Datastore::Init"
	);

public:
/** @name Operations */ /// @{
	/**
		Execute command.

		@note IO::PropType::identity is implicit, and any props not
		supplied for a particular object are ignored when loading
		props.

		@par
		@note Storage-unsupplied requested props of set
		IO::PropTypeBit::base are initialized with dummy values and
		set to IO::PropState::modified. Primary and auxiliary prop
		states are not changed if the datastore does not have storage
		for them.

		@param prop_types Props to load for all objects.
	*/
	exec_result_type
	operator()(
		IO::PropTypeBit prop_types
	) noexcept;
/// @}
};

/**
	Datastore store command.
*/
class Store final
	: public Cmd::Unit<Store>
{
	HORD_CMD_IMPL_BOILERPLATE(
		Store,
		"Cmd::Datastore::Store"
	);

private:
	unsigned m_num_objects_stored{0u};
	unsigned m_num_props_stored{0u};

public:
/** @name Properties */ /// @{
	/**
		Get the number of objects stored.
	*/
	unsigned
	num_objects_stored() const noexcept {
		return m_num_objects_stored;
	}

	/**
		Get the number of props stored.
	*/
	unsigned
	num_props_stored() const noexcept {
		return m_num_props_stored;
	}
/// @}

/** @name Operations */ /// @{
	/**
		Execute command.

		@note Only props that are IO::PropState::modified will be
		stored.
	*/
	exec_result_type
	operator()() noexcept;
/// @}
};

/** @} */ // end of doc-group cmd_datastore

} // namespace Datastore

/** @cond INTERNAL */
HORD_CMD_IMPL_ENSURE_TRAITS(Cmd::Datastore::Init);
HORD_CMD_IMPL_ENSURE_TRAITS(Cmd::Datastore::Store);
/** @endcond */ // INTERNAL

} // namespace Cmd
} // namespace Hord
