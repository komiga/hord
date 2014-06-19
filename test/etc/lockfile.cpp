
#include <Hord/aux.hpp>
#include <Hord/String.hpp>
#include <Hord/Log.hpp>
#include <Hord/LockFile.hpp>

#include <cassert>
#include <cstdlib>
#include <iostream>

signed
main(
	signed argc,
	char const* const argv[]
) {
	Hord::LockFile lock{HORD_STR_LIT("./dummy_lockfile")};
	if (1 < argc) {
		lock.set_path(Hord::String{argv[1]});
	}

	std::cout
		<< "1: exit (without explicit release())\n"
		<< "2: abort (idem)\n"
		<< "3: assert (idem)\n"
		<< "4: acquire\n"
		<< "5: release\n"
		<< "q: return normally\n"
		<< "\n"
	;

	signed x = '\0';
	while (x != 'q') {
		std::cout
			<< "is_active() = " << std::boolalpha << lock.is_active() << '\n'
			<< "> "
		;
		do {
			x = std::cin.get();
		} while (x == '\n');
		std::cout << '\n';
		switch (x) {
		case '1':
			std::exit(0);
			break;

		case '2':
			std::abort();
			break;

		case '3':
			assert(false);
			break;

		case '4':
			try {
				lock.acquire();
			} catch (Hord::Error const& err) {
				Hord::Log::report_error(err);
			}
			break;

		case '5':
			lock.release();
			break;

		default:
			break;
		}
	}

	lock.release();
	return 0;
}
