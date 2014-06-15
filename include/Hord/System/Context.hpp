/**
@file System/Context.hpp
@brief Context class.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_SYSTEM_CONTEXT_HPP_
#define HORD_SYSTEM_CONTEXT_HPP_

#include <Hord/config.hpp>
#include <Hord/aux.hpp>
#include <Hord/Hive/Defs.hpp>
#include <Hord/Hive/Unit.hpp>
#include <Hord/IO/Datastore.hpp>
#include <Hord/System/Driver.hpp>

#include <utility>

namespace Hord {
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
class Context final {
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

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~Context() noexcept;

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
};

/** @} */ // end of doc-group system

} // namespace System
} // namespace Hord

#endif // HORD_SYSTEM_CONTEXT_HPP_
