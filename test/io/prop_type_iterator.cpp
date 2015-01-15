
#include <Hord/utility.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/PropTypeIterator.hpp>

#include <duct/debug.hpp>
#include <duct/utility.hpp>

#include <iostream>

using namespace duct::enum_ops;
using Type = Hord::IO::PropType;
using TypeBit = Hord::IO::PropTypeBit;

void
test(
	char const* const name,
	TypeBit const types
) {
	std::cout << name << ":\n";
	TypeBit reconstructed = TypeBit::none;
	for (auto const type : types) {
		std::cout
			<< "   "
			<< Hord::IO::get_prop_type_name(type)
			<< '\n'
		;
		reconstructed |= Hord::IO::prop_type_bit(type);
	}
	std::cout << '\n';
	DUCT_ASSERTE(types == reconstructed);
}

signed
main() {
	test("none", TypeBit::none);
	test("all" , TypeBit::all);
	test("base", TypeBit::base);
	test("data", TypeBit::data);
	test("identity", TypeBit::identity);
	test("metadata", TypeBit::metadata);
	test("scratch", TypeBit::scratch);
	test("primary", TypeBit::primary);
	test("auxiliary", TypeBit::auxiliary);
	return 0;
}
