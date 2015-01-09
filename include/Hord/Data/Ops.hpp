/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Data operations.
@ingroup data
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/Data/ValueRef.hpp>

#include <iosfwd>

namespace Hord {
namespace Data {

// Forward declarations
struct ValueRefPrinter;

/**
	@addtogroup data
	@{
*/

/**
	Value printer.
*/
struct ValueRefPrinter {
/** @name Properties */ /// @{
	/**
		Value.

		@note The referred object must exist as long as the printer.
	*/
	Data::ValueRef const& value;
/// @}

private:
	ValueRefPrinter() = delete;

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~ValueRefPrinter() noexcept = default;

	/** Copy constructor. */
	ValueRefPrinter(ValueRefPrinter const&) noexcept = default;
	/** Move constructor. */
	ValueRefPrinter(ValueRefPrinter&&) noexcept = default;

	/**
		Constructor with value.
	*/
	ValueRefPrinter(
		Data::ValueRef const& value
	) noexcept
		: value(value)
	{}
/// @}
};

/**
	Write value to stream as string.
*/
std::ostream&
operator<<(
	std::ostream& stream,
	Data::ValueRefPrinter const& printer
);

/** @} */ // end of doc-group data

} // namespace Data
} // namespace Hord
