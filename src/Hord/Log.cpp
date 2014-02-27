
#include <Hord/String.hpp>
#include <Hord/Log.hpp>

#include <duct/IO/multistream.hpp>

#include <utility>
#include <locale>
#include <chrono>
#include <iomanip>
#include <iostream>

namespace Hord {
namespace Log {

/*thread_local*/ Controller
s_controller{
	true, true, true,
	HORD_STR_LIT("")
};

namespace {
enum : unsigned {
	MC_INDEX_STDOUT = 0u,
	MC_INDEX_FILE = 1u
};

static char const
s_iso8601_fmt[]{"%FT%T%z"};

static Pre const
s_prefix_for_type[]{
	// NB: StreamType::general currently routes to Pre::none
	Pre::none,
	Pre::debug,
	Pre::error
};

static char const* const
s_prefix_strings[]{
	HORD_STR_LIT(""),
	HORD_STR_LIT("<dbg> "),
	HORD_STR_LIT("<err> ")
};
} // anonmyous namespace

OutputStream&
operator<<(
	OutputStream& stream,
	Pre prefix
) {
	if (Pre::current == prefix) {
		prefix = s_prefix_for_type[
			stream.get_type()
		];
	}
	if (Pre::none != prefix) {
		stream << s_prefix_strings[static_cast<unsigned>(prefix)];
	}
	return stream;
}

// class Controller implementation

Controller::Controller(
	bool const enable_stdout,
	bool const enable_file,
	bool const enable_datastore,
	String path
) noexcept
	: m_flags()
	, m_file_path(std::move(path))
	, m_file_stream()
	, m_mc_buffer()
	, m_mc_vectors{
		{{
			enable_stdout ? &std::cout : nullptr,
			nullptr
		}},
		{{
			enable_stdout ? &std::clog : nullptr,
			nullptr
		}},
		{{
			enable_stdout ? &std::cerr : nullptr,
			nullptr
		}}
	}
	, m_mc_streambufs{
		// general
		{m_mc_vectors[0], m_mc_buffer, STREAMBUF_BUFFER_SIZE},
		// debug
		{m_mc_vectors[1], m_mc_buffer, STREAMBUF_BUFFER_SIZE},
		// error
		{m_mc_vectors[2], m_mc_buffer, STREAMBUF_BUFFER_SIZE}
	}
{
	m_flags.set(Flag::write_stdout, enable_stdout);
	this->file(enable_file);
	this->datastore(enable_datastore);
}

Controller::~Controller() noexcept {
	close_file();
}

// NB: std::ofstream discards data when closed, so this is
// unnecessary 
void
Controller::disable_file_stream() noexcept {
	for (auto& vec : m_mc_vectors) {
		vec[MC_INDEX_FILE] = nullptr;
	}
}

void
Controller::enable_file_stream() noexcept {
	for (auto& vec : m_mc_vectors) {
		vec[MC_INDEX_FILE] = &m_file_stream;
	}
}

bool
Controller::open_file() {
	if (
		m_flags.test(Flag::write_file) &&
		!m_file_stream.is_open() &&
		!m_file_path.empty()
	) {
		m_file_stream.clear(
			m_file_stream.rdstate() &
			~std::ios_base::failbit
		);
		m_file_stream.open(
			m_file_path,
			std::ios_base::out |
			std::ios_base::app
		);
		if (m_file_stream.fail()) {
			disable_file_stream();
			Log::acquire(Log::error)
				<< "failed to open log file: '"
				<< m_file_path
				<< "'\n"
			;
			return false;
		} else {
			enable_file_stream();
			std::time_t const systime = std::chrono::system_clock::to_time_t(
				std::chrono::system_clock::now()
			);
			Log::acquire()
				<< '['
				<< std::put_time(std::localtime(&systime), s_iso8601_fmt)
				<< "] opened log file: '"
				<< m_file_path
				<< "'\n"
			;
		}
	}
	return m_file_stream.is_open();
}

bool
Controller::close_file() {
	bool success = true;
	if (m_file_stream.is_open()) {
		m_file_stream.clear(
			m_file_stream.rdstate() &
			~std::ios_base::failbit
		);
		std::time_t const systime = std::chrono::system_clock::to_time_t(
			std::chrono::system_clock::now()
		);
		Log::acquire()
			<< '['
			<< std::put_time(std::localtime(&systime), s_iso8601_fmt)
			<< "] closing log file: '"
			<< m_file_path
			<< "'\n"
		;
		disable_file_stream();
		m_file_stream.close();
		if (m_file_stream.fail()) {
			Log::acquire(Log::error)
				<< "failed to close log file\n"
			;
			success = false;
		}
	}
	return success;
}

void
Controller::stdout(
	bool const enable
) noexcept {
	static std::ostream*
	s_stdout_pointers[]{
		&std::cout,
		&std::clog,
		&std::cerr
	};

	if (
		enable &&
		!m_flags.test(Flag::write_stdout)
	) {
		std::size_t index = 0;
		for (auto& vec : m_mc_vectors) {
			vec[MC_INDEX_STDOUT] = s_stdout_pointers[index++];
		}
	} else if (
		!enable &&
		m_flags.test(Flag::write_stdout)
	) {
		for (auto& vec : m_mc_vectors) {
			vec[MC_INDEX_STDOUT] = nullptr;
		}
	}
	m_flags.set(Flag::write_stdout, enable);
}

bool
Controller::file(
	bool const enable
) noexcept {
	m_flags.set(Flag::write_file, enable);
	return enable
		? open_file()
		: close_file()
	;
}

void
Controller::datastore(
	bool const enable
) noexcept {
	// TODO
	m_flags.set(Flag::write_datastore, enable);
}

bool
Controller::set_file_path(
	String path
) noexcept {
	close_file();
	m_file_path.assign(std::move(path));
	return open_file();
}

// class OutputStream implementation

OutputStream::OutputStream(
	StreamType const type,
	bool const put_prefix
) noexcept
	: base(
		&s_controller.m_mc_streambufs[
			static_cast<unsigned>(type)
		]
	)
	, m_type(type)
{
	if (put_prefix) {
		(*this) << Pre::current;
	}
}

} // namespace Log
} // namespace Hord
