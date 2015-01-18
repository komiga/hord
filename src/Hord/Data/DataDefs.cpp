/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/config.hpp>
#include <Hord/String.hpp>
#include <Hord/Data/Defs.hpp>

namespace Hord {
namespace Data {

char const*
get_value_type_name(
	Data::ValueType const value_type
) noexcept {
	switch (value_type) {
	case Data::ValueType::null		: return HORD_STR_LIT("null");
	case Data::ValueType::dynamic	: return HORD_STR_LIT("dynamic");
	case Data::ValueType::integer	: return HORD_STR_LIT("integer");
	case Data::ValueType::decimal	: return HORD_STR_LIT("decimal");
	case Data::ValueType::object_id	: return HORD_STR_LIT("object_id");
	case Data::ValueType::string	: return HORD_STR_LIT("string");
	default: return HORD_STR_LIT("INVALID");
	}
}

char const*
get_size_name(
	Data::Size const size
) noexcept {
	switch (size) {
	case Data::Size::b8 : return HORD_STR_LIT("b8");
	case Data::Size::b16: return HORD_STR_LIT("b16");
	case Data::Size::b32: return HORD_STR_LIT("b32");
	case Data::Size::b64: return HORD_STR_LIT("b64");
	default: return HORD_STR_LIT("INVALID");
	}
}

} // namespace Data
} // namespace Hord
