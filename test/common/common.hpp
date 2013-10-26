
#ifndef HORD_TEST_COMMON_COMMON_HPP_
#define HORD_TEST_COMMON_COMMON_HPP_

#include <Hord/Error.hpp>

#include <iostream>

void
report_error(
	Hord::Error const& e
) {
	std::cerr
		<< '[' << Hord::get_error_name(e.get_code()) << ']'
		<< '\n' << e.get_message()
		<< '\n'
	<< std::endl;
}

#endif // HORD_TEST_COMMON_COMMON_HPP_
