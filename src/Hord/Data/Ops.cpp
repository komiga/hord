/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/utility.hpp>
#include <Hord/Data/Defs.hpp>
#include <Hord/Data/ValueRef.hpp>
#include <Hord/Data/Ops.hpp>
#include <Hord/Object/Ops.hpp>

#include <ostream>

#include <Hord/detail/gr_ceformat.hpp>

namespace Hord {
namespace Data {

namespace {
static constexpr ceformat::Format const
	s_fmt_integer_signed{"%d"},
	s_fmt_integer_unsigned{"%u"},
	s_fmt_decimal{"%.6g"}
;
} // anonymous namespace

std::ostream&
operator<<(
	std::ostream& stream,
	Data::ValueRefPrinter const& printer
) {
	Data::ValueRef const& value = printer.value;
	switch (value.type.type()) {
	case Data::ValueType::null:
		stream << "null";
		break;

	case Data::ValueType::dynamic:
		// NB: Should never happen
		stream << "(dynamic)";
		break;

	case Data::ValueType::integer:
		if (enum_bitand(value.type.flags(), Data::ValueFlag::integer_signed)) {
			ceformat::write<s_fmt_integer_signed>(stream, value.integer_signed());
		} else {
			ceformat::write<s_fmt_integer_unsigned>(stream, value.integer_unsigned());
		}
		break;

	case Data::ValueType::decimal:
		ceformat::write<s_fmt_decimal>(stream, value.decimal());
		break;

	case Data::ValueType::object_id:
		stream << value.data.object_id;
		break;

	case Data::ValueType::string:
		stream.write(value.data.string, value.size);
		break;
	}
	return stream;
}

Data::Type
parse_value(
	ValueParser& parser,
	unsigned const string_size,
	char const* const string
) noexcept {
	parser.flags = VPFLAG_NONE;
	parser.decimal_pos = 0;
	parser.exponent_pos = 0;
	auto it = string;
	auto const end = string + string_size;
	char c;
	if (!string || string_size == 0) {
		return {Data::ValueType::null};
	}
	for (; it < end; ++it) {
		c = *it;
		switch (c) {
		case 'n':
			if (
				it == string && string_size == 4 &&
				string_equal(string_size, string + 1, 3, "ull")
			) {
				return {Data::ValueType::null};
			} else {
				goto l_value_string;
			}
			break;

		case '-': case '+':
			if (
				(parser.flags & VPFLAG_EXPONENT_SIGN) ||
				(~parser.flags & VPFLAG_EXPONENT && parser.flags & VPFLAG_SIGN) ||
				(parser.flags & VPFLAG_EXPONENT_NUMERAL)
			) {
				goto l_value_string;
			}
			if (parser.flags & VPFLAG_EXPONENT) {
				parser.flags |= VPFLAG_EXPONENT_SIGN;
			} else if (c == '-') {
				parser.flags |= VPFLAG_SIGN | VPFLAG_IS_NEGATIVE;
			} else {
				parser.flags |= VPFLAG_SIGN;
			}
			break;

		case '.':
			if (
				(parser.flags & VPFLAG_EXPONENT) ||
				(parser.flags & VPFLAG_DECIMAL)
			) {
				goto l_value_string;
			}
			parser.flags |= VPFLAG_DECIMAL;
			parser.decimal_pos = end - it;
			break;

		case 'e': case 'E':
			if (parser.flags & VPFLAG_EXPONENT) {
				goto l_value_string;
			}
			parser.flags |= VPFLAG_EXPONENT;
			parser.exponent_pos = end - it;
			break;

		default:
			if ('0' <= c && c <= '9') {
				if (parser.flags & VPFLAG_EXPONENT) {
					parser.flags |= VPFLAG_EXPONENT_NUMERAL;
				} else if (parser.flags & VPFLAG_DECIMAL) {
					parser.flags |= VPFLAG_NUMERAL | VPFLAG_DECIMAL_NUMERAL;
				} else {
					parser.flags |= VPFLAG_NUMERAL;
				}
			} else {
				goto l_value_string;
			}
			break;
		}
	}

	// Integer or decimal
	if (
		// Missing numeral part in number
		(~parser.flags & VPFLAG_NUMERAL) ||
		// Missing numeral part after number exponent
		(parser.flags & VPFLAG_EXPONENT && ~parser.flags & VPFLAG_EXPONENT_NUMERAL)
	) {
		goto l_value_string;
	}
	if (parser.flags & VPFLAG_DECIMAL || parser.flags & VPFLAG_EXPONENT) {
		return {
			Data::ValueType::decimal,
			Data::Size::b64
		};
	} else {
		return {
			Data::ValueType::integer,
			parser.flags & VPFLAG_IS_NEGATIVE
			? Data::ValueFlag::integer_signed
			: Data::ValueFlag::none,
			Data::Size::b64
		};
	}

l_value_string:
	return {
		Data::ValueType::string,
		Data::Size::b32
	};
}

} // namespace Data
} // namespace Hord
