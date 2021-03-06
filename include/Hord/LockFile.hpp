/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief LockFile class.
@ingroup etc
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/String.hpp>

namespace Hord {

// Forward declarations
class LockFile;

/**
	@addtogroup etc
	@{
*/

/**
	Lock file.

	@remarks An active lock should be automatically released if the
	program is abnormally terminated.
*/
class LockFile final {
private:
	using handle_type = signed;

	enum : handle_type {
		NULL_HANDLE = -1
	};

	handle_type m_handle{NULL_HANDLE};
	String m_path{};

	LockFile(LockFile const&) = delete;
	LockFile& operator=(LockFile const&) = delete;

public:
/** @name Special member functions */ /// @{
	/**
		Destructor.

		@remarks Will automatically @c release() the lock.
	*/
	~LockFile();

	/** Default constructor. */
	LockFile() = default;
	/** Move constructor. */
	LockFile(LockFile&&) = default;
	/** Move assignment operator. */
	LockFile& operator=(LockFile&&) = default;

	/**
		Constructor with path.

		@note This does not call @c acquire(); that must be done
		manually.
	*/
	explicit
	LockFile(
		String path
	) noexcept;
/// @}

/** @name Properties */ /// @{
	/**
		Check if lock is active.
	*/
	bool
	is_active() const noexcept {
		return NULL_HANDLE != m_handle;
	}

	/**
		Set path.

		@throws Error{ErrorCode::lockfile_immutable}
		If the lock is active.
	*/
	void
	set_path(
		String path
	);

	/**
		Get path.
	*/
	String const&
	path() const noexcept {
		return m_path;
	}
/// @}

/** @name Operations */ /// @{
	/**
		Acquire lock.

		@throws Error{ErrorCode::lockfile_acquire_failed}
		If the lock could not be acquired.
	*/
	void
	acquire();

	/**
		Release lock.
	*/
	void
	release() noexcept;
/// @}
};

/** @} */ // end of doc-group etc

} // namespace Hord
