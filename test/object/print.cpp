
#include <Hord/utility.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Object/Ops.hpp>
#include <Hord/Table/UnitBasic.hpp>

#include <iostream>

using namespace Hord;

signed
main() {
	auto node = Table::UnitBasic::info.construct(
		Table::ID{0x2a2a2a2a},
		Object::ID_NULL
	);
	node->set_slug("sluggy");

	std::cout
		<< "node: " << *node << "\n"
		<< "id: " << Object::IDPrinter{*node} << "\n"
	;

	return 0;
}
