/**
@file Log.hpp
@brief Logging utilities.

@author Tim Howard
@copyright 2013-2014 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_LOG_HPP_
#define HORD_LOG_HPP_

#include <Hord/config.hpp>
#include <Hord/String.hpp>

#include <duct/StateStore.hpp>
#include <duct/IO/multistream.hpp>

#include <fstream>
#include <ostream>

namespace Hord {
namespace Log {

// TODO: Common output sentinels (e.g., current time in ISO-8601)
// TODO: Logging interface like exception throwing

// TODO: Datastores need to logify by roots.. somehow.
// Driver should probably manage that. Actually, Driver will
// probably be doing most of the Datastore logging anyways.

// TODO: Need to handle multi-threaded output to file stream properly
// FIXME: thread_local requires g++ 4.8!? (In Clang!)

// Forward declarations
class Controller;
class OutputStream;

/**
	@addtogroup etc
	@{
*/
/**
	@addtogroup log
	@{
*/

/**
	Stream type.
*/
enum StreamType : unsigned {
	general = 0u,
	debug,
	error,

/** @cond INTERNAL */
	LAST
/** @endcond */ // INTERNAL
};

/** Stream prefixes. */
enum class Pre : unsigned {
	general = 0u,
	debug,
	error,

	none,
	current,

/** @cond INTERNAL */
	LAST,
	LAST_TYPE = error
/** @endcond */ // INTERNAL
};

/** @cond INTERNAL */
enum : std::size_t {
	STREAMBUF_BUFFER_SIZE = 2048u
};

extern /*thread_local*/ Controller
s_controller;
/** @endcond */ // INTERNAL

/**
	Get the log controller.
*/
inline Controller&
get_controller() noexcept {
	return s_controller;
}

/**
	Log controller.
*/
class Controller final {
private:
	friend class OutputStream;

	/**
		State flags.
	*/
	enum class Flag : unsigned {
		write_stdout = 1 << 0,
		write_file = 1 << 1,
		write_datastore = 1 << 2
	};

	duct::StateStore<Flag> m_flags;
	String m_file_path;

	std::ofstream m_file_stream;
	char m_mc_buffer[STREAMBUF_BUFFER_SIZE];
	duct::IO::multicast_vector_type m_mc_vectors[
		static_cast<unsigned>(StreamType::LAST)
	];
	duct::IO::multistreambuf m_mc_streambufs[
		static_cast<unsigned>(StreamType::LAST)
	];

	/** Disable multicast to log file stream. */
	void
	disable_file_stream() noexcept;
	/** Enable multicast to log file stream. */
	void
	enable_file_stream() noexcept;

	/**
		Open the log file stream.

		@returns Whether the operation succeeded.
	*/
	bool
	open_file();

	/**
		Close the log file stream.

		@returns Whether the operation succeeded.
	*/
	bool
	close_file();

	Controller(Controller&&) = delete;
	Controller(Controller const&) = delete;
	Controller& operator=(Controller const&) = delete;
	Controller& operator=(Controller&&) = delete;

public:
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with flags and log file path.

		@param enable_stdout Enable or disable standard output.
		@param enable_file Enable or disable log file output.
		@param enable_datastore Enable or disable datastore-local
		file output.
		@param path Log file path.
	*/
	Controller(
		bool const enable_stdout,
		bool const enable_file,
		bool const enable_datastore,
		String path
	) noexcept;

	/** Destructor. */
	~Controller() noexcept;
/// @}

/** @name Configuration */ /// @{
	/**
		Enable or disable standard output.

		@param enable Whether to enable or disable output.
	*/
	void
	stdout(
		bool const enable
	) noexcept;

	/**
		Enable or disable log file output.

		@returns
		- @c true if the operation succeeded (i.e., opening or closing
		  the log file stream);
		- @c false if the operation failed.
		@param enable Whether to enable or disable output.
	*/
	bool
	file(
		bool const enable
	) noexcept;

	/**
		Enable or disable datastore-local file output.

		@param enable Whether to enable or disable output.
	*/
	void
	datastore(
		bool const enable
	) noexcept;

	/**
		Set the log file path.

		@remarks If the file stream is currently open, it is closed.
		If opening on @a path fails, @c false is returned, but file
		output is still enabled. If log file output is disabled,
		this only assigns the log file path.

		@returns
		- @c true if the file stream was successfully opened;
		- @c false if the file path could not be opened for writing.
		@param path Log file path.
	*/
	bool
	set_file_path(
		String path
	) noexcept;

	/**
		Get log file path.
	*/
	String const&
	get_file_path() const noexcept {
		return m_file_path;
	}
/// @}
};

/**
	Log output stream.
*/
class OutputStream final
	: public std::ostream
{
private:
	using base = std::ostream;

	StreamType const m_type;

	OutputStream() = delete;
	OutputStream(OutputStream const&) = delete;
	OutputStream& operator=(OutputStream const&) = delete;
	OutputStream& operator=(OutputStream&&) = delete;

public:
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with type and streambuf.

		@param type Stream type.
		@param put_prefix Whether to write Pre::current.
	*/
	OutputStream(
		StreamType const type,
		bool const put_prefix
	) noexcept;

	// FIXME: Defect in libstdc++ 4.7.3: basic_ostream
	// move ctor is deleted and swap() is not defined
	/**
		Move constructor.

		@param other Stream to take ownership of.
	*/
	OutputStream(
		OutputStream&& other
	) noexcept
		//: base(std::move(other))
		: base(other.rdbuf())
		, m_type(other.m_type)
	{}

	/** Destructor. */
	~OutputStream() noexcept override {
		// Force-write streambuf to multicast streams
		static_cast<duct::IO::multistreambuf*>(
			this->rdbuf()
		)->multicast();
	}
/// @}

/** @name Properties */ /// @{
	/**
		Get stream type.
	*/
	StreamType
	get_type() const noexcept {
		return m_type;
	}
/// @}
};

/**
	Acquire log output stream for type.

	@param type Stream type.
*/
inline OutputStream
acquire(
	StreamType const type = Log::StreamType::general,
	bool const put_prefix = true
) noexcept {
	return OutputStream(type, put_prefix);
}

/** @cond INTERNAL */
OutputStream&
operator<<(
	OutputStream& stream,
	Pre prefix
);
/** @endcond */ // INTERNAL

/** @} */ // end of doc-group log
/** @} */ // end of doc-group etc

} // namespace Log
} // namespace Hord

#endif // HORD_LOG_HPP_
