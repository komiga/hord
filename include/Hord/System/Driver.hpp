/**
@file System/Driver.hpp
@brief Driver.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_SYSTEM_DRIVER_HPP_
#define HORD_SYSTEM_DRIVER_HPP_

#include <Hord/config.hpp>
#include <Hord/aux.hpp>
#include <Hord/String.hpp>
#include <Hord/System/IDGenerator.hpp>
#include <Hord/IO/Datastore.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Rule/Defs.hpp>
#include <Hord/Hive/Defs.hpp>
#include <Hord/Hive/Unit.hpp>
#include <Hord/Cmd/Defs.hpp>

#include <duct/cc_unique_ptr.hpp>

#include <utility>

namespace Hord {
namespace System {

// Forward declarations
class Driver;

/**
	@addtogroup system
	@{
*/

/**
	Driver.
*/
class Driver final {
public:
	/** Datastore-hive pair. */
	struct datastore_hive_pair final {
		/** Datastore. */
		duct::cc_unique_ptr<IO::Datastore> datastore;

		/** Hive. */
		Object::UPtr hive;
	};

	/** Datastore-hive collection. */
	using hive_map_type
	= aux::unordered_map<
		Hive::ID,
		datastore_hive_pair
	>;

private:
	using object_type_map_type
	= aux::unordered_map<
		Object::Type,
		Object::type_info const&
	>;

	using command_table_vector_type
	= aux::vector<
		Cmd::type_info_table const*
	>;

	System::IDGenerator m_id_generator;
	object_type_map_type m_object_types;
	command_table_vector_type m_command_tables;
	hive_map_type m_hives;

	Driver() = delete;
	Driver(Driver const&) = delete;
	Driver& operator=(Driver const&) = delete;
	Driver& operator=(Driver&&) = delete;

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~Driver() noexcept;

	/** Move constructor. */
	Driver(Driver&&);

	/**
		Constructor with type registration.
	*/
	Driver(
		bool const register_standard_object_types
	);
/// @}

/** @name Properties */ /// @{
	/**
		Get ID generator.
	*/
	System::IDGenerator&
	get_id_generator() noexcept {
		return m_id_generator;
	}

	/**
		Get hive collection.
	*/
	hive_map_type&
	get_hives() noexcept {
		return m_hives;
	}
/// @}

/** @name Type information */ /// @{
	/**
		Register object type information.

		@remarks %Client-defined objects must be registered for them
		to be recognized during (de)serialization.

		@throws Error{ErrorCode::driver_object_type_shared}
		If the type has already been registered.
	*/
	void
	register_object_type(
		Object::type_info const& type_info
	);

	/**
		Register rule type information.

		@throws Error{ErrorCode::driver_rule_type_zero_permitted_types}
		If @c type_info.permitted_types is @c 0.

		@throws Error{ErrorCode::driver_object_type_shared}
		If the type has already been registered.
	*/
	void
	register_rule_type(
		Rule::type_info const& type_info
	);

	/**
		Get object type information.

		@returns The object type info, or @c nullptr if the type was
		not registered.
		@param type %Object type.
	*/
	Object::type_info const*
	get_object_type_info(
		Object::Type const type
	) const noexcept;

	/**
		Register command type information table.

		@throws Error{ErrorCode::driver_command_table_range_invalid}
		If the type range:

		- is degenerate
		  (<code>table.range_begin > table.range_end</code>);
		- contains no types (<code>table.size() == 0</code>);
		- intersects the range reserved for stages; or
		- intersects the range reserved for standard commands.

		@throws Error{ErrorCode::driver_command_table_range_shared}
		If the table's type range intersects with the type range of a
		previously-registered table.

		@param table Command type info table.
	*/
	void
	register_command_type_table(
		Cmd::type_info_table const& table
	);

	/**
		Get command type information.

		@returns The command type info, or @c nullptr if the type was
		not supplied in a registered type info table.
		@param type Command type.
	*/
	Cmd::type_info const*
	get_command_type_info(
		Cmd::Type const type
	) const noexcept;
/// @}

/** @name Operations */ /// @{
	/**
		Placehold hive.

		@warning @c ErrorCode::driver_hive_root_shared is only caused
		by string comparison. It is possible for multiple hives to
		share the same actual directory (which is often used by a
		datastore to lock access to the hive), but the second one
		to attempt deserialization will fail
		with @c ErrorCode::driver_datastore_locked.

		@post Emplaced hive with @a root_path
		and @c IO::StorageState::placeholder.

		@throws Error{ErrorCode::driver_hive_root_empty}
		If <code>root_path.empty() == true</code>.

		@throws Error{ErrorCode::driver_hive_root_shared}
		If the root path has already been placeheld.

		@throws Error{ErrorCode::driver_datastore_construct_failed}
		If the datastore failed to construct (see
		IO::Datastore::type_info::construct()).

		@returns The placeheld hive.
		@param hive_type Hive type to placehold.
		@param type_info Datastore type for the hive.
		@param root_path Root path.
	*/
	System::Driver::datastore_hive_pair&
	placehold_hive(
		Hive::Type const hive_type,
		IO::Datastore::type_info const& type_info,
		String root_path
	);
/// @}

/** @name Collections */ /// @{
	/**
		Lookup a hive.

		@returns Iterator to datastore-hive pair, or hive
		collection's end iterator if @a id was not found.
		@param id Hive ID.
	*/
	System::Driver::hive_map_type::iterator
	find_hive(
		Hive::ID const id
	) noexcept {
		return m_hives.find(id);
	}
/// @}
};

/** @} */ // end of doc-group system

} // namespace System
} // namespace Hord

#endif // HORD_SYSTEM_DRIVER_HPP_
