/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Context class.
@ingroup context
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/aux.hpp>
#include <Hord/IO/Datastore.hpp>
#include <Hord/System/Driver.hpp>

#include <utility>
#include <exception>

namespace Hord {

namespace Cmd {

struct TypeInfo;
class UnitBase;

} // namespace Cmd

namespace System {

// Forward declarations
class Context;

/**
	@addtogroup context
	@{
*/

/**
	%Context.

	@warning The context requires the attached datastore to exist
	as long as the context exists.
*/
class Context {
private:
	System::Driver& m_driver;
	IO::Datastore& m_datastore;

	Context() = delete;
	Context(Context const&) = delete;
	Context& operator=(Context const&) = delete;
	Context& operator=(Context&&) = delete;

	Context(
		System::Driver& driver,
		IO::Datastore& datastore
	);

protected:
/** @name Implementation */ /// @{
	/**
		notify_exception() implementation.
	*/
	virtual void
	notify_exception_impl(
		Cmd::UnitBase const& command,
		Cmd::TypeInfo const& type_info,
		std::exception_ptr eptr
	) noexcept = 0;

	/**
		notify_complete() implementation.
	*/
	virtual void
	notify_complete_impl(
		Cmd::UnitBase const& command,
		Cmd::TypeInfo const& type_info
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

		@throws Error{ErrorCode::context_invalid_datastore}
		If @a datastore_id does not exist within @a driver.
	*/
	Context(
		System::Driver& driver,
		IO::Datastore::ID const datastore_id
	);
/// @}

/** @name Properties */ /// @{
	/**
		Get driver (mutable).
	*/
	System::Driver&
	driver() noexcept {
		return m_driver;
	}

	/**
		Get driver.
	*/
	System::Driver const&
	driver() const noexcept {
		return m_driver;
	}

	/**
		Get datastore (mutable).
	*/
	IO::Datastore&
	datastore() noexcept {
		return m_datastore;
	}

	/**
		Get datastore.
	*/
	IO::Datastore const&
	datastore() const noexcept {
		return m_datastore;
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
		Cmd::TypeInfo const& type_info,
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
		Cmd::TypeInfo const& type_info
	) noexcept {
		notify_complete_impl(command, type_info);
	}
/// @}
};

/** @} */ // end of doc-group context

} // namespace System
} // namespace Hord
