/**
@file Error.hpp
@brief Generic exception classes.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_ERROR_HPP_
#define HORD_ERROR_HPP_

#include "./config.hpp"
#include "./common_enums.hpp"
#include "./String.hpp"

#include <exception>

namespace Hord {

// Forward declarations
class Error;

/**
	@addtogroup error
	@{
*/

/**
	Get the name of an error.
	@returns C-string containing the name of @a error_code or "INVALID" if somehow @a error_code is not actually an ErrorCode.
	@param error_code ErrorCode.
*/
char const* get_error_name(ErrorCode const error_code) noexcept;

/**
	Error.
*/
class Error final : public std::exception {
private:
	ErrorCode const m_errc;
	String const m_msg;

	Error()=delete;
	Error(Error const&)=delete;
	Error& operator=(Error const&)=delete;

public:
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with error code and message.
		@param errc Error code.
		@param msg Error message.
	*/
	Error(ErrorCode errc, String msg) noexcept
		: std::exception{}
		, m_errc{errc}
		, m_msg{std::move(msg)}
	{}
	/** Move constructor. */
	Error(Error&&)=default;
	/** Destructor. */
	~Error() override=default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Error& operator=(Error&&)=default;
/// @}

/** @name Properties */ /// @{
	/**
		Get error code.
		@returns The error code.
	*/
	ErrorCode error_code() const noexcept { return m_errc; }
	/**
		Get error message.
		@returns The error message.
	*/
	String const& what_str() const noexcept { return m_msg; }
	/**
		Get C-string error message.
		@note This is UTF-8 encoded (per String).
		@returns C-string of the error message.
	*/
	char const* what() const noexcept override { return m_msg.c_str(); }
/// @}
};

/** @} */ // end of doc-group error

} // namespace Hord

#endif // HORD_ERROR_HPP_
