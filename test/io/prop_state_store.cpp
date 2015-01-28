
#include <Hord/utility.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/PropStateStore.hpp>

#include <cassert>
#include <iostream>
#include <bitset>

using Hord::enum_cast;
using Type = Hord::IO::PropType;
using TypeBit = Hord::IO::PropTypeBit;
using State = Hord::IO::PropState;
using Store = Hord::IO::PropStateStore;

void
print_types(
	char const* const msg,
	TypeBit const types
) {
	std::cout << msg << ":\n";
	if (TypeBit::none == types) {
		std::cout << "   none";
	} else {
		for (auto const type : types) {
			std::cout
				<< "   "
				<< Hord::IO::get_prop_type_name(type)
				<< '\n'
			;
		}
	}
	std::cout << '\n';
}

void
print_store(
	Store const& s,
	Type const t = static_cast<Type>(0u)
) {
	bool const typed = 0u != enum_cast(t);
	unsigned const value
		= typed
		? enum_cast(s.state(t))
		: s.value()
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

void
assignments(
	Store& s,
	unsigned const initial
) {
	s.assign(Type::identity, State::modified);
	s.assign(Type::metadata, State::modified);
	s.assign(Type::scratch, State::modified);
	s.assign(Type::primary, State::modified);
	s.assign(Type::auxiliary, State::modified);
	print_store(s);
	print_types("after assign; initialized", s.initialized_of(TypeBit::all));
	print_types("after assign; uninitialized", s.uninitialized_of(TypeBit::all));

	assert(TypeBit::all == s.initialized_of(TypeBit::all));
	assert(TypeBit::none  == s.uninitialized_of(TypeBit::all));
	assert(!s.all_original());
	assert(s.any_modified());

	s.assign(Type::identity, State::original);
	s.assign(Type::metadata, State::original);
	s.assign(Type::scratch, State::original);
	s.assign(Type::primary, State::original);
	s.assign(Type::auxiliary, State::original);
	print_store(s);

	assert(s.all_original());
	assert(!s.any_modified());

	// Back to initial value when resetting
	s.reset_all();
	assert(s.value() == initial);

	s.assign_all(State::original);
	print_store(s);

	assert(s.all_original());
	assert(!s.any_modified());
}

signed
main() {
	Store
		s_all{true, true},
		s_neither{false, false}
	;
	unsigned const
		value_init_all = s_all.value(),
		value_init_neither = s_neither.value()
	;

	// Constructed state
	assert(s_all.all_uninitialized());
	assert(s_neither.all_uninitialized());
	assert(TypeBit::none == s_all.initialized_of(TypeBit::all));
	assert(TypeBit::all  == s_all.uninitialized_of(TypeBit::all));

	assert(0u == value_init_all);

	// Unchanged when resetting while all_uninitialized()
	s_all.reset_all();
	assert(s_all.value() == value_init_all);

	s_neither.reset_all();
	assert(s_neither.value() == value_init_neither);

	// Unsupplied props are unassignable
	s_neither.reset(Type::primary);
	s_neither.assign(Type::auxiliary, State::modified);
	assert(s_neither.value() == value_init_neither);

	// Traits
	std::cout << "s_all:\n";
	print_store(s_all);
	print_types("after init; initialized", s_all.initialized_of(TypeBit::all));
	print_types("after init; uninitialized", s_all.uninitialized_of(TypeBit::all));

	assert(s_all.supplies(Type::primary));
	assert(s_all.supplies(Type::auxiliary));
	assert(s_all.supplies_all(TypeBit::all));
	assert(!s_all.all_original());
	assert(!s_all.any_modified());
	assert(!s_all.has(Type::primary, State::modified));
	assert(!s_all.has(Type::primary, State::original));
	assert(!s_all.has(Type::auxiliary, State::modified));
	assert(!s_all.has(Type::auxiliary, State::original));

	assignments(s_all, value_init_all);

	std::cout << "\ns_neither:\n";
	print_store(s_neither);
	print_types("after init; initialized", s_neither.initialized_of(TypeBit::all));
	print_types("after init; uninitialized", s_neither.uninitialized_of(TypeBit::all));

	assert(!s_neither.supplies(Type::primary));
	assert(!s_neither.supplies(Type::auxiliary));
	assert(s_all.supplies_all(TypeBit::base));
	assert(!s_neither.all_original());
	assert(!s_neither.any_modified());
	assert(!s_neither.has(Type::primary, State::modified));
	assert(!s_neither.has(Type::auxiliary, State::modified));
	assert(s_neither.has(Type::primary, State::original));
	assert(s_neither.has(Type::auxiliary, State::original));

	assignments(s_neither, value_init_neither);

	return 0;
}
