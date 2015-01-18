
#include <Hord/utility.hpp>
#include <Hord/Data/Defs.hpp>
#include <Hord/Data/ValueRef.hpp>

#include <duct/debug.hpp>

#include <limits>

using namespace Hord;

using s8  = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;

using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

template<class T>
constexpr T lim_min() {
	return std::numeric_limits<T>::min();
}
template<class T>
constexpr T lim_max() {
	return std::numeric_limits<T>::max();
}

void
check_morph(
	Data::ValueRef const& value,
	Data::ValueRef const& expected
) {
	auto morphed = value;
	morphed.morph(expected.type);
	DUCT_ASSERTE(morphed == expected);
}

signed
main() {
	{// downsize max u64
		Data::ValueRef const v{lim_max<u64>()};
		check_morph(v, {lim_max<u8>()});
		check_morph(v, {lim_max<u16>()});
		check_morph(v, {lim_max<u32>()});

		check_morph(v, {lim_max<s8>()});
		check_morph(v, {lim_max<s16>()});
		check_morph(v, {lim_max<s32>()});
		check_morph(v, {lim_max<s64>()});
	}

	{// upsize max u8
		Data::ValueRef const v{lim_max<u8>()};
		check_morph(v, {u16{lim_max<u8>()}});
		check_morph(v, {u32{lim_max<u8>()}});
		check_morph(v, {u64{lim_max<u8>()}});

		check_morph(v, {lim_max<s8>()});
		check_morph(v, {s16{lim_max<u8>()}});
		check_morph(v, {s32{lim_max<u8>()}});
		check_morph(v, {s64{lim_max<u8>()}});
	}

	{// downsize max s64
		Data::ValueRef const v{lim_max<s64>()};
		check_morph(v, {lim_max<u8>()});
		check_morph(v, {lim_max<u16>()});
		check_morph(v, {lim_max<u32>()});
		check_morph(v, {u64{lim_max<s64>()}});

		check_morph(v, {lim_max<s8>()});
		check_morph(v, {lim_max<s16>()});
		check_morph(v, {lim_max<s32>()});
	}

	{// downsize min s64
		Data::ValueRef const v{lim_min<s64>()};
		check_morph(v, {u8{0}});
		check_morph(v, {u16{0}});
		check_morph(v, {u32{0}});
		check_morph(v, {u64{0}});

		check_morph(v, {lim_min<s8>()});
		check_morph(v, {lim_min<s16>()});
		check_morph(v, {lim_min<s32>()});
	}
	return 0;
}
