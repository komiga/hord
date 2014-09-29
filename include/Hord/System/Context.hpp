/**
@file System/Context.hpp
@brief Context class.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/aux.hpp>
#include <Hord/Hive/Defs.hpp>
#include <Hord/Hive/Unit.hpp>
#include <Hord/IO/Datastore.hpp>
#include <Hord/System/Driver.hpp>

#include <utility>
#include <exception>

namespace Hord {

namespace Cmd {

struct type_info;
class UnitBase;

} // namespace Cmd

namespace System {

// Forward declarations
class Context;

/**
	@addtogroup system
	@{
*/

/**
	%Context.

	@warning The context requires the attached datastore and hive
	exist as long as the context exists.

	@remarks This class provides an interface to objects of a
	specific hive.
*/
class Context {
private:
	System::Driver& m_driver;
	IO::Datastore& m_datastore;
	Hive::Unit& m_hive;

	Context() = delete;
	Context(Context const&) = delete;
	Context& operator=(Context const&) = delete;
	Context& operator=(Context&&) = delete;

	Context(
		System::Driver& driver,
		System::Driver::datastore_hive_pair& hive_pair
	);

protected:
/** @name Implementation */ /// @{
	/**
		notify_exception() implementation.
	*/
	virtual void
	notify_exception_impl(
		Cmd::UnitBase const& command,
		Cmd::type_info const& type_info,
		std::exception_ptr eptr
	) noexcept = 0;

	/**
		notify_complete() implementation.
	*/
	virtual void
	notify_complete_impl(
		Cmd::UnitBase const& command,
		Cmd::type_info const& type_info
	) noexcept = 0;
/// @}

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	virtual ~Context() noexcept = 0;

	/** Move constructor. */
	Context(Context&&) noexcept;

	/**
		Constructor with properties.

		@throws Error{ErrorCode::context_invalid_hive}
		If @a hive_id does not exist within @a driver.
	*/
	Context(
		System::Driver& driver,
		Hive::ID const hive_id
	);
/// @}

/** @name Properties */ /// @{
	/**
		Get driver.
	*/
	System::Driver&
	get_driver() noexcept {
		return m_driver;
	}

	/**
		Get driver.
	*/
	System::Driver const&
	get_driver() const noexcept {
		return m_driver;
	}

	/**
		Get datastore.
	*/
	IO::Datastore&
	get_datastore() noexcept {
		return m_datastore;
	}

	/**
		Get datastore.
	*/
	IO::Datastore const&
	get_datastore() const noexcept {
		return m_datastore;
	}

	/**
		Get hive (mutable).
	*/
	Hive::Unit&
	get_hive() noexcept {
		return m_hive;
	}

	/**
		Get hive.
	*/
	Hive::Unit const&
	get_hive() const noexcept {
		return m_hive;
	}
/// @}

/** @name Operations */ /// @{
	/**
		Notify the context that a command caught an exception.

		@note This should not be called directly. Cmd::Unit exposes
		its own function to call this.

		@param command Command unit.
		@param type_info Command type info.
		@param eptr Exception pointer.
	*/
	void
	notify_exception(
		Cmd::UnitBase const& command,
		Cmd::type_info const& type_info,
		std::exception_ptr&& eptr
	) noexcept {
		notify_exception_impl(command, type_info, std::move(eptr));
	}

	/**
		Notify the context that a command completed.

		@note This should not be called directly. It is called
		automatically by Cmd::Unit::commit() after unit commit.

		@param command Command unit.
		@param type_info Command type info.
	*/
	void
	notify_complete(
		Cmd::UnitBase const& command,
		Cmd::type_info const& type_info
	) noexcept {
		notify_complete_impl(command, type_info);
	}
/// @}
};

/** @} */ // end of doc-group system

} // namespace System
} // namespace Hord
