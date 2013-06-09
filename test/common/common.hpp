
#ifndef HORD_TEST_COMMON_COMMON_HPP_
#define HORD_TEST_COMMON_COMMON_HPP_

#include <Hord/Error.hpp>

#include <iostream>

void report_error(Hord::Error const& e) {
	std::cerr
		<<'['<<Hord::get_error_name(e.error_code())<<']'
		<<'\n'<<e.what_str()
		<<'\n'
	<<std::endl;
}

#endif // HORD_TEST_COMMON_COMMON_HPP_
