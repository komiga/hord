/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Data operations.
@ingroup data
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/Data/Defs.hpp>
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

/**
	Value parser flags.
*/
enum : unsigned {
	VPFLAG_NONE				= 0,
	VPFLAG_SIGN				= 1 << 0,
	VPFLAG_NUMERAL			= 1 << 1,
	VPFLAG_DECIMAL			= 1 << 2,
	VPFLAG_DECIMAL_NUMERAL	= 1 << 3,
	VPFLAG_EXPONENT			= 1 << 4,
	VPFLAG_EXPONENT_SIGN	= 1 << 5,
	VPFLAG_EXPONENT_NUMERAL	= 1 << 6,
	VPFLAG_IS_NEGATIVE		= 1 << 7,
};

/**
	Value parser data.
*/
struct ValueParser {
	unsigned flags;
	unsigned decimal_pos;
	unsigned exponent_pos;
};

/**
	Parse value from char array.
*/
Data::Type
parse_value(
	ValueParser& parser,
	unsigned const string_size,
	char const* const string
) noexcept;

/** @} */ // end of doc-group data

} // namespace Data
} // namespace Hord
