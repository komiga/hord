/**
@file System/Driver.hpp
@brief Driver.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/aux.hpp>
#include <Hord/String.hpp>
#include <Hord/utility.hpp>
#include <Hord/System/IDGenerator.hpp>
#include <Hord/IO/Datastore.hpp>
#include <Hord/Object/Defs.hpp>

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
	/** Datastore collection. */
	using datastore_map_type
	= aux::unordered_map<
		IO::Datastore::ID,
		IO::Datastore::UPtr
	>;

private:
	using object_type_map_type
	= aux::unordered_map<
		Object::Type,
		Object::type_info const&
	>;

	System::IDGenerator m_id_generator;
	object_type_map_type m_object_types;
	IO::Datastore::ID m_datastore_id_gen;
	datastore_map_type m_datastores;

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
		Get datastores collection.
	*/
	System::Driver::datastore_map_type&
	get_datastores() noexcept {
		return m_datastores;
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
		Get object type information.

		@returns The object type info, or @c nullptr if the type was
		not registered.
	*/
	Object::type_info const*
	get_object_type_info(
		Object::Type const type
	) const noexcept;
/// @}

/** @name Operations */ /// @{
	/**
		Placehold datastore.

		@warning @c ErrorCode::driver_datastore_root_shared is only caused
		by string comparison. It is possible for multiple datastores to
		share the same actual path (which is often used by a datastore
		to lock access), but the second one to attempt deserialization
		will fail with @c ErrorCode::driver_datastore_locked.

		@post Emplaced datastore with @a root_path
		and @c IO::StorageState::placeholder.

		@throws Error{ErrorCode::driver_datastore_root_empty}
		If <code>root_path.empty() == true</code>.

		@throws Error{ErrorCode::driver_datastore_root_shared}
		If the root path has already been placeheld.

		@throws Error{ErrorCode::driver_datastore_construct_failed}
		If the datastore failed to construct (see
		IO::Datastore::type_info::construct()).

		@returns The placeheld datastore.
	*/
	IO::Datastore&
	placehold_datastore(
		IO::Datastore::type_info const& type_info,
		String root_path
	);
/// @}

/** @name Collections */ /// @{
	/**
		Lookup a datastore by ID.
	*/
	IO::Datastore*
	find_datastore(
		IO::Datastore::ID const id
	) noexcept {
		auto const it = m_datastores.find(id);
		return m_datastores.end() != it
			? it->second.get()
			: nullptr
		;
	}

	/**
		Lookup a datastore by root path.
	*/
	IO::Datastore*
	find_datastore(
		String const& root_path
	) noexcept {
		return find_datastore(hash_string(root_path));
	}
/// @}
};

/** @} */ // end of doc-group system

} // namespace System
} // namespace Hord
