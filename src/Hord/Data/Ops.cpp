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

// class ValueRef implementation

#define HORD_SCOPE_CLASS ValueRef

namespace {
static constexpr ceformat::Format const
	s_fmt_integer_signed{"%d"},
	s_fmt_integer_unsigned{"%u"},
	s_fmt_decimal{"%f"}
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
		stream << "(null)";
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

#undef HORD_SCOPE_CLASS

} // namespace Data
} // namespace Hord
