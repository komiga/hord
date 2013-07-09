/**
@file LockFile.hpp
@brief LockFile class.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_LOCKFILE_HPP_
#define HORD_LOCKFILE_HPP_

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

	handle_type m_handle;
	String m_path;

	enum : handle_type {
		NULL_HANDLE = -1
	};

	LockFile() = delete;
	LockFile(LockFile const&) = delete;
	LockFile& operator=(LockFile const&) = delete;

public:
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with path.

		@note This does not call @c acquire(); that must be done
		manually.

		@param path File path to lock.
	*/
	explicit
	LockFile(
		String path
	) noexcept;

	/** Move constructor. */
	LockFile(LockFile&&);
	/**
		Destructor.

		@remarks Will automatically @c release() the lock.
	*/
	~LockFile();
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	LockFile& operator=(LockFile&&);
/// @}

/** @name Properties */ /// @{
	/**
		Check if lock is active.

		@returns @c true if the lock is active; @c false otherwise.
	*/
	bool
	is_active() const noexcept {
		return NULL_HANDLE != m_handle;
	}

	/**
		Set path.

		@throws Error{ErrorCode::lockfile_immutable}
		If the lock is active.

		@param path New path.
	*/
	void
	set_path(
		String path
	);

	/**
		Get path.
		@returns The lock's path.
	*/
	String const&
	get_path() const noexcept {
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

#endif // HORD_LOCKFILE_HPP_
