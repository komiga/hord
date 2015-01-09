/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/utility.hpp>
#include <Hord/Data/Defs.hpp>
#include <Hord/Data/ValueRef.hpp>

#include <cmath>

#include <Hord/detail/gr_ceformat.hpp>

namespace Hord {
namespace Data {

// class ValueRef implementation

#define HORD_SCOPE_CLASS ValueRef

bool
ValueRef::operator==(
	Data::ValueRef const& rhs
) const noexcept {
	if (type != rhs.type) {
		return false;
	}
	switch (type.type()) {
	case Data::ValueType::null:
		return true;
	case Data::ValueType::dynamic:
		return true;
	case Data::ValueType::integer:
		return integer_signed() == rhs.integer_signed();
	case Data::ValueType::decimal:
		return std::fabs(decimal() - rhs.decimal()) < 0.0001f;
	case Data::ValueType::object_id:
		return data.object_id == rhs.data.object_id;
	case Data::ValueType::string:
		return string_equal(size, data.string, rhs.size, rhs.data.string);
	}
}

#undef HORD_SCOPE_CLASS

} // namespace Data
} // namespace Hord
