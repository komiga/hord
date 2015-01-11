/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief %Datastore commands.
@ingroup cmd_datastore
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/Unit.hpp>

namespace Hord {
namespace Cmd {
namespace Datastore {

// Forward declarations
class Init;
class Load;
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
		@note Storage-unsupplied requested props are initialized with
		dummy values (or unchanged from the initial state for primary
		and auxiliary states) and set to IO::PropState::modified.

		@param prop_types Props to load for all objects.
	*/
	exec_result_type
	operator()(
		IO::PropTypeBit prop_types
	) noexcept;
/// @}
};

/**
	Datastore load command.
*/
class Load final
	: public Cmd::Unit<Load>
{
	HORD_CMD_IMPL_BOILERPLATE(
		Load,
		"Cmd::Datastore::Load"
	);

	friend class Init;

protected:
	static unsigned
	load_or_initialize(
		IO::Datastore& datastore,
		Hord::Object::Unit& object,
		IO::StorageInfo const& sinfo,
		IO::PropTypeBit prop_types
	);

private:
	unsigned m_num_objects_loaded{0u};
	unsigned m_num_props_loaded{0u};

public:
/** @name Properties */ /// @{
	/**
		Get the number of objects loaded.
	*/
	unsigned
	num_objects_loaded() const noexcept {
		return m_num_objects_loaded;
	}

	/**
		Get the number of props loaded.
	*/
	unsigned
	num_props_loaded() const noexcept {
		return m_num_props_loaded;
	}
/// @}

public:
/** @name Operations */ /// @{
	/**
		Load uninitialized props for all objects.

		@note Any props not supplied for a particular object are
		ignored.

		@param prop_types Props to load for all objects.
	*/
	exec_result_type
	operator()(
		IO::PropTypeBit prop_types
	) noexcept;

	/**
		Load uninitialized props for an object.

		@note Any props not supplied by the object are ignored.
	*/
	exec_result_type
	operator()(
		Hord::Object::ID object_id,
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
		Store all modified props for all objects.
	*/
	exec_result_type
	operator()() noexcept;

	/**
		Store all modified props for the specified object.
	*/
	exec_result_type
	operator()(
		Hord::Object::ID object_id
	) noexcept;
/// @}
};

/** @} */ // end of doc-group cmd_datastore

} // namespace Datastore

/** @cond INTERNAL */
HORD_CMD_IMPL_ENSURE_TRAITS(Cmd::Datastore::Init);
HORD_CMD_IMPL_ENSURE_TRAITS(Cmd::Datastore::Load);
HORD_CMD_IMPL_ENSURE_TRAITS(Cmd::Datastore::Store);
/** @endcond */ // INTERNAL

} // namespace Cmd
} // namespace Hord
