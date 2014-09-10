/**
@file Cmd/Hive.hpp
@brief %Hive commands.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/Unit.hpp>

namespace Hord {
namespace Cmd {
namespace Hive {

// Forward declarations
class Init;
class StoreAll;

/**
	@addtogroup cmd
	@{
*/
/**
	@addtogroup cmd_hive
	@{
*/

/**
	Hive init command.
*/
class Init final
	: public Cmd::Unit<Init>
{
	HORD_CMD_IMPL_BOILERPLATE(
		Init,
		"Cmd::Hive::Init"
	)

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

		@param prop_types Properties to load for all objects.
	*/
	result_type
	operator()(
		IO::PropTypeBit prop_types
	) noexcept;
/// @}
};

/**
	Hive store-all command.
*/
class StoreAll final
	: public Cmd::Unit<StoreAll>
{
	HORD_CMD_IMPL_BOILERPLATE(
		StoreAll,
		"Cmd::Hive::StoreAll"
	)

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
	result_type
	operator()() noexcept;
/// @}
};

/** @} */ // end of doc-group cmd_hive
/** @} */ // end of doc-group cmd

} // namespace Hive

HORD_CMD_IMPL_ENSURE_TRAITS(Cmd::Hive::Init);
HORD_CMD_IMPL_ENSURE_TRAITS(Cmd::Hive::StoreAll);

} // namespace Cmd
} // namespace Hord
