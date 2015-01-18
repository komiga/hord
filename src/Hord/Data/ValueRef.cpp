/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/utility.hpp>
#include <Hord/Data/Defs.hpp>
#include <Hord/Data/ValueRef.hpp>
#include <Hord/Data/Ops.hpp>

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
	if (type.type() != rhs.type.type()) {
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

inline constexpr static std::int64_t
s64_min(
	Data::Size const size
) {
	return (std::int64_t{1} << 63) | (std::int64_t{1} << ((8 << enum_cast(size)) - 1));
}

inline constexpr static std::int64_t
s64_max(
	Data::Size const size
) {
	return ~(static_cast<std::int64_t>(~0ll) << ((8 << enum_cast(size)) - 1));
}

inline constexpr static std::uint64_t
u64_max(
	Data::Size const size
) {
	return ~((static_cast<std::uint64_t>(~0ull) << ((8 << enum_cast(size)) - 1)) << 1);
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
		switch (Data::size_meta(new_type.size())) {
		case 1: size = min_ce(size, static_cast<unsigned>(static_cast<std::uint8_t >(~0u))); break;
		case 2: size = min_ce(size, static_cast<unsigned>(static_cast<std::uint16_t>(~0u))); break;
		case 4: size = min_ce(size, static_cast<unsigned>(static_cast<std::uint32_t>(~0u))); break;
		}
	} else if (new_type.type() == Data::ValueType::integer) {
		std::int64_t xs;
		std::uint64_t xu;
		if (enum_cast(type.flags() & ValueFlag::integer_signed)) {
			switch (type.size()) {
			case Data::Size::b8 : xs = data.s8 ; break;
			case Data::Size::b16: xs = data.s16; break;
			case Data::Size::b32: xs = data.s32; break;
			case Data::Size::b64: xs = data.s64; break;
			}
			xu = static_cast<std::uint64_t>(max_ce(std::int64_t{0}, xs));
		} else {
			switch (type.size()) {
			case Data::Size::b8 : xu = data.u8 ; break;
			case Data::Size::b16: xu = data.u16; break;
			case Data::Size::b32: xu = data.u32; break;
			case Data::Size::b64: xu = data.u64; break;
			}
			xs = static_cast<std::int64_t>(
				min_ce(static_cast<std::uint64_t>(s64_max(Data::Size::b64)), xu)
			);
		}
		if (enum_cast(new_type.flags() & ValueFlag::integer_signed)) {
			xs = max_ce(
				s64_min(new_type.size()),
				min_ce(xs, s64_max(new_type.size()))
			);
			switch (new_type.size()) {
			case Data::Size::b8 : data.s8  = static_cast<std::int8_t >(xs); break;
			case Data::Size::b16: data.s16 = static_cast<std::int16_t>(xs); break;
			case Data::Size::b32: data.s32 = static_cast<std::int32_t>(xs); break;
			case Data::Size::b64: data.s64 = xs; break;
			}
		} else {
			xu = min_ce(xu, u64_max(new_type.size()));
			switch (new_type.size()) {
			case Data::Size::b8 : data.u8  = static_cast<std::uint8_t >(xu); break;
			case Data::Size::b16: data.u16 = static_cast<std::uint16_t>(xu); break;
			case Data::Size::b32: data.u32 = static_cast<std::uint32_t>(xu); break;
			case Data::Size::b64: data.u64 = xu; break;
			}
		}
	}
	type = new_type;
}

void
ValueRef::read_from_string(
	unsigned const string_size,
	char const* const string,
	bool const assume_signed
) noexcept {
	Data::ValueParser parser{};
	type = Data::parse_value(parser, string_size, string);
	size = 0;
	data.s64 = 0;
	switch (type.type()) {
	case Data::ValueType::integer:
		if (parser.flags & VPFLAG_IS_NEGATIVE || assume_signed) {
			type = {
				Data::ValueType::integer,
				Data::ValueFlag::integer_signed,
				Data::Size::b64
			};
			data.s64 = std::int64_t{std::strtoll(string, nullptr, 10)};
		} else {
			data.u64 = std::uint64_t{std::strtoull(string, nullptr, 10)};
		}
		break;

	case Data::ValueType::decimal:
		data.f64 = std::strtod(string, nullptr);
		break;
			data.u64 = std::uint64_t{std::strtoull(string, nullptr, 10)};

	case Data::ValueType::object_id:
		data.object_id = Object::ID{
			static_cast<Object::IDValue>(std::strtoull(string, nullptr, 16))
		};
		break;

	case Data::ValueType::string:
		size = string_size;
		data.string = string;
		break;

	// Any other type will yield null
	default: break;
	}
}

#undef HORD_SCOPE_CLASS

} // namespace Data
} // namespace Hord
