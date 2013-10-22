
#include <Hord/utility.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Object/Ops.hpp>
#include <Hord/Node/Unit.hpp>

#include <iostream>

using namespace Hord;

signed
main() {
	Node::Unit node{
		Node::ID{0x2a2a2a2a},
		Object::NULL_ID
	};
	node.set_slug("sluggy");

	std::cout
		<< "node: " << node << "\n"
		<< "id: " << Object::IDPrinter{node} << "\n"
	;

	return 0;
}