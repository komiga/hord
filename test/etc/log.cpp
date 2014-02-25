
#include <Hord/String.hpp>
#include <Hord/Log.hpp>

#include <iostream>

#include "../common/common.hpp"

namespace Log = Hord::Log;

signed
main() {
	Log::Controller& controller = Log::get_controller();
	controller.set_file_path("./log.log");
	controller.file(true);
	{
		Log::acquire(Log::general, false)
			<< "test " << 3.141592 << '\n'
			<< Log::Pre::current << "current prefix\n"
			<< Log::Pre::debug   << "debug prefix\n"
			<< Log::Pre::error   << "error prefix\n"
		;
	}
	controller.file(false);
	{
		Log::acquire(Log::debug)
			<< "stdlog (file disabled)\n"
		;
	}
	controller.file(true);
	{
		Log::acquire(Log::error)
			<< "stderr (file enabled)\n"
		;
	}
	return 0;
}
