
#include <Hord/utility.hpp>
#include <Hord/IO/Prop.hpp>

#include <cassert>
#include <iostream>
#include <bitset>

using Hord::enum_cast;
using Type = Hord::IO::PropType;
using State = Hord::IO::PropState;
using Store = Hord::IO::PropStateStore;

void
print_store(
	Store const& s,
	Type const t = static_cast<Type>(0u)
) {
	bool const typed = 0u != enum_cast(t);
	unsigned const value
		= typed
		? enum_cast(s.get_state(t))
		: s.get_value()
	;

	if (typed) {
		std::cout
			<< Hord::IO::get_prop_type_name(t) << ": "
			<< std::bitset<4u>{value}
		;
	} else {
		std::cout
			<< "value: "
			<< std::bitset<4u>{value >> (4 << 2)} << ' '
			<< std::bitset<4u>{value >> (3 << 2)} << ' '
			<< std::bitset<4u>{value >> (2 << 2)} << ' '
			<< std::bitset<4u>{value >> (1 << 2)} << ' '
			<< std::bitset<4u>{value >> (0 << 2)} << ' '
		;
	}
	std::cout << '\n';
}

signed
main() {
	Store
		s_all{true, true},
		s_neither{false, false}
	;
	unsigned const
		value_init_all = s_all.get_value(),
		value_init_neither = s_neither.get_value()
	;

	std::cout << "s_all:\n";
	print_store(s_all);
	print_store(s_all, Type::primary);
	print_store(s_all, Type::auxiliary);

	std::cout << "\ns_neither:\n";
	print_store(s_neither);
	print_store(s_neither, Type::primary);
	print_store(s_neither, Type::auxiliary);

	std::cout << '\n';

	// Constructed state
	assert(s_all.all_uninitialized());
	assert(s_neither.all_uninitialized());

	assert(0u == value_init_all);

	assert(!s_all.all_original());
	assert(!s_all.any_modified());

	assert(!s_all.has(Type::primary, State::original));
	assert(!s_all.has(Type::auxiliary, State::original));

	assert(s_neither.has(Type::primary, State::original));
	assert(s_neither.has(Type::auxiliary, State::original));

	assert(s_all.is_supplied(Type::primary));
	assert(s_all.is_supplied(Type::auxiliary));

	assert(!s_neither.is_supplied(Type::primary));
	assert(!s_neither.is_supplied(Type::auxiliary));

	// Unchanged when resetting while all_uninitialized()
	s_all.reset_all();
	assert(s_all.get_value() == value_init_all);

	s_neither.reset_all();
	assert(s_neither.get_value() == value_init_neither);

	// Unsupplied props are unassignable
	s_neither.reset(Type::primary);
	s_neither.assign(Type::auxiliary, State::modified);
	assert(s_neither.get_value() == value_init_neither);

	// Assignments
	s_all.assign(Type::identity, State::original);
	s_all.assign(Type::metadata, State::original);
	s_all.assign(Type::scratch, State::original);
	s_all.assign(Type::primary, State::original);
	s_all.assign(Type::auxiliary, State::original);

	print_store(s_all);

	assert(s_all.all_original());
	assert(!s_all.any_modified());

	// Back to initial value when resetting
	s_all.reset_all();
	assert(s_all.get_value() == value_init_all);

	// More assignments
	s_all.assign(Type::identity, State::modified);
	s_all.assign(Type::metadata, State::modified);
	s_all.assign(Type::scratch, State::modified);
	s_all.assign(Type::primary, State::modified);
	s_all.assign(Type::auxiliary, State::modified);

	print_store(s_all);

	assert(!s_all.all_original());
	assert(s_all.any_modified());

	return 0;
}
