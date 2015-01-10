/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/utility.hpp>
#include <Hord/Data/Defs.hpp>
#include <Hord/Data/ValueRef.hpp>

#include <cmath>
#include <cstdlib>

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

inline static std::int64_t
value_clamp_signed(
	std::int64_t const value,
	Data::Size const size
) noexcept {
	auto const min = std::int64_t{1} << ((enum_cast(size) + 3) - 1);
	return max_ce(min, min_ce(value, ~(~std::int64_t{0} << (enum_cast(size) + 3))));
}

inline static std::uint64_t
value_clamp_unsigned(
	std::uint64_t const value,
	Data::Size const size
) noexcept {
	return max_ce(std::uint64_t{0}, min_ce(value, ~std::uint64_t{0} >> (enum_cast(size) + 3)));
}

void
ValueRef::morph(
	Data::Type const new_type
) noexcept {
	if (new_type == type || new_type.type() == Data::ValueType::dynamic) {
		return;
	} else if (new_type.type() != type.type()) {
		data.u64 = 0;
	} else if (Data::type_properties(new_type).flags & VTP_DYNAMIC_SIZE) {
		switch (Data::data_size_dynamic(new_type.size())) {
		case 1: size = min_ce(size, static_cast<unsigned>(~std::uint8_t {0})); break;
		case 2: size = min_ce(size, static_cast<unsigned>(~std::uint16_t{0})); break;
		case 4: size = min_ce(size, ~unsigned{0}); break;
		}
	} else if (new_type.type() == Data::ValueType::integer) {
		if (enum_bitand(new_type.flags(), ValueFlag::integer_signed)) {
			std::int64_t x;
			switch (type.size()) {
			case Data::Size::b8 : x = data.s8 ; break;
			case Data::Size::b16: x = data.s16; break;
			case Data::Size::b32: x = data.s32; break;
			case Data::Size::b64: x = data.s64; break;
			}
			x = value_clamp_signed(x, new_type.size());
			switch (new_type.size()) {
			case Data::Size::b8 : x = data.s8  = static_cast<std::int8_t >(x); break;
			case Data::Size::b16: x = data.s16 = static_cast<std::int16_t>(x); break;
			case Data::Size::b32: x = data.s32 = static_cast<std::int32_t>(x); break;
			case Data::Size::b64: x = data.s64 = x; break;
			}
		} else {
			std::uint64_t x;
			switch (type.size()) {
			case Data::Size::b8 : x = data.u8 ; break;
			case Data::Size::b16: x = data.u16; break;
			case Data::Size::b32: x = data.u32; break;
			case Data::Size::b64: x = data.u64; break;
			}
			x = value_clamp_unsigned(x, new_type.size());
			switch (new_type.size()) {
			case Data::Size::b8 : x = data.u8  = static_cast<std::uint8_t >(x); break;
			case Data::Size::b16: x = data.u16 = static_cast<std::uint16_t>(x); break;
			case Data::Size::b32: x = data.u32 = static_cast<std::uint32_t>(x); break;
			case Data::Size::b64: x = data.u64 = x; break;
			}
		}
	}
	type = new_type;
}

void
ValueRef::read_from_string(
	unsigned const string_size,
	char const* const string
) noexcept {
	enum : unsigned {
		PART_SIGN				= 1 << 0,
		PART_NUMERAL			= 1 << 1,
		PART_DECIMAL			= 1 << 2,
		PART_DECIMAL_NUMERAL	= 1 << 3,
		PART_EXPONENT			= 1 << 4,
		PART_EXPONENT_SIGN		= 1 << 5,
		PART_EXPONENT_NUMERAL	= 1 << 6,
	};

	auto it = string;
	auto const end = string + string_size;
	char c;
	unsigned parts = 0;
	if (!string || string_size == 0) {
		goto l_assign_null;
	}
	for (; it < end; ++it) {
		c = *it;
		switch (c) {
		case 'n':
			if (
				it == string && string_size == 4 &&
				string_equal(string_size, string + 1, 3, "ull")
			) {
				goto l_assign_null;
			} else {
				goto l_assign_string;
			}
			break;

		case '-': case '+':
			if (
				(parts & PART_EXPONENT_SIGN) ||
				(~parts & PART_EXPONENT && parts & PART_SIGN) ||
				(parts & PART_EXPONENT_NUMERAL)
			) {
				goto l_assign_string;
			}
			if (parts & PART_EXPONENT) {
				parts |= PART_EXPONENT_SIGN;
			} else {
				parts |= PART_SIGN;
			}
			break;

		case '.':
			if (
				(parts & PART_EXPONENT) ||
				(parts & PART_DECIMAL)
			) {
				goto l_assign_string;
			}
			parts |= PART_DECIMAL;
			break;

		case 'e': case 'E':
			if (parts & PART_EXPONENT) {
				goto l_assign_string;
			}
			parts |= PART_EXPONENT;
			break;

		default:
			if ('0' <= c && c <= '9') {
				if (parts & PART_EXPONENT) {
					parts |= PART_EXPONENT_NUMERAL;
				} else if (parts & PART_DECIMAL) {
					parts |= PART_NUMERAL | PART_DECIMAL_NUMERAL;
				} else {
					parts |= PART_NUMERAL;
				}
			} else {
				goto l_assign_string;
			}
			break;
		}
	}

	// Assign to integer or decimal
	if (
		// missing numeral part in number
		(~parts & PART_NUMERAL) ||
		// missing numeral part after number exponent
		(parts & PART_EXPONENT && ~parts & PART_EXPONENT_NUMERAL)
	) {
		goto l_assign_string;
	}
	if (parts & PART_DECIMAL || parts & PART_EXPONENT) {
		type = {
			Data::ValueType::decimal,
			Data::Size::b64
		};
		data.f64 = std::strtod(string, nullptr);
	} else {
		if (string[0] == '-') {
			type = {
				Data::ValueType::integer,
				Data::ValueFlag::integer_signed,
				Data::Size::b64
			};
			data.s64 = std::int64_t{std::strtoll(string, nullptr, 10)};
		} else {
			type = {
				Data::ValueType::integer,
				Data::Size::b64
			};
			data.u64 = std::uint64_t{std::strtoull(string, nullptr, 10)};
		}
	}
	return;

l_assign_null:
	type = Data::ValueType::null;
	size = 0;
	data.s64 = 0;
	return;

l_assign_string:
	type = {Data::ValueType::string, Data::Size::b32};
	size = string_size;
	data.string = string;
	return;
}

#undef HORD_SCOPE_CLASS

} // namespace Data
} // namespace Hord
