/**
@file IO/PropStream.hpp
@brief PropStream classes.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_IO_PROPSTREAM_HPP_
#define HORD_IO_PROPSTREAM_HPP_

#include <Hord/config.hpp>
#include <Hord/serialization.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/Prop.hpp>

#include <iosfwd>

namespace Hord {
namespace IO {

// Forward declarations
class Datastore; // external
class PropStream;
class InputPropStream;
class OutputPropStream;

/**
	@addtogroup io
	@{
*/
/**
	@addtogroup prop
	@{
*/

/**
	Base prop stream.

	@warning This class only provides the base interface for
	InputPropStream and OutputPropStream and does not supply a
	virtual destructor for base-class destruction.

	@remarks Acquiring a weak stream does not call any acquire
	function on datastore, and releasing a weakly-acquired stream
	will not call any release function on the datastore.

	@sa IO::PropInfo
*/
class PropStream {
protected:
	/** %Datastore. */
	IO::Datastore& m_datastore;

	/** Prop info. */
	IO::PropInfo const m_info;

	/** Whether the current stream is not owned by the datastore. */
	bool m_weak;

private:
	PropStream() = delete;
	PropStream(PropStream const&) = delete;
	PropStream& operator=(PropStream const&) = delete;
	PropStream& operator=(PropStream&&) = delete;

protected:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~PropStream() noexcept = default;

	/** Move constructor. */
	PropStream(PropStream&&) noexcept = default;

	/**
		Constructor with datastore and prop info.
	*/
	PropStream(
		IO::Datastore& datastore,
		IO::PropInfo info
	)
		: m_datastore(datastore)
		, m_info(std::move(info))
	{}
/// @}

public:
/** @name Properties */ /// @{
	/**
		Get datastore.
	*/
	IO::Datastore&
	get_datastore() noexcept {
		return m_datastore;
	}

	/**
		Get info.
	*/
	IO::PropInfo const&
	get_info() const noexcept {
		return m_info;
	}

	/**
		Get prop type.
	*/
	IO::PropType
	get_type() const noexcept {
		return m_info.prop_type;
	}

	/**
		Check whether the stream is not owned by the datastore.
	*/
	bool
	is_weak() const noexcept {
		return m_weak;
	}
/// @}
};

/**
	Input prop stream.
*/
class InputPropStream final
	: public IO::PropStream
{
private:
	using base = IO::PropStream;

	std::istream* m_stream;

	InputPropStream() = delete;
	InputPropStream(InputPropStream const&) = delete;
	InputPropStream& operator=(InputPropStream const&) = delete;
	InputPropStream& operator=(InputPropStream&&) = delete;

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~InputPropStream() noexcept;

	/** Move constructor. */
	InputPropStream(InputPropStream&&) noexcept;

	/**
		Constructor with datastore and prop info.
	*/
	InputPropStream(
		IO::Datastore& datastore,
		IO::PropInfo info
	);
/// @}

/** @name Properties */ /// @{
	/**
		Get raw input stream.

		@warning If this happens to be called on a destroyed prop
		stream, the return value will be a null reference and
		terrific things will happen.
	*/
	std::istream&
	get_stream();
/// @}

/** @name Operations */ /// @{
	/**
		Make an input serializer for the prop stream.
	*/
	InputSerializer
	make_serializer() noexcept {
		return make_input_serializer(get_stream());
	}

	/**
		See IO::Datastore::acquire_input_stream().

		@pre Not acquired.
		@post Acquired.
	*/
	void
	acquire();

	/**
		Acquire non-datastore stream.

		@pre Not acquired.
		@post Acquired.
	*/
	void
	acquire_weak(
		std::istream& stream
	) noexcept;

	/**
		See IO::Datastore::release_input_stream().

		@post Not acquired.
	*/
	void
	release();
/// @}
};

/**
	Output prop stream.
*/
class OutputPropStream final
	: public IO::PropStream
{
private:
	using base = IO::PropStream;

	std::ostream* m_stream;

	OutputPropStream() = delete;
	OutputPropStream(OutputPropStream const&) = delete;
	OutputPropStream& operator=(OutputPropStream const&) = delete;
	OutputPropStream& operator=(OutputPropStream&&) = delete;

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~OutputPropStream() noexcept;

	/** Move constructor. */
	OutputPropStream(OutputPropStream&&) noexcept;

	/**
		Constructor with datastore and prop info.
	*/
	OutputPropStream(
		IO::Datastore& datastore,
		IO::PropInfo info
	);
/// @}

public:
/** @name Properties */ /// @{
	/**
		Get raw output stream.

		@warning If this happens to be called on a destroyed prop
		stream, the return value will be a null reference and
		terrific things will happen.
	*/
	std::ostream&
	get_stream();
/// @}

/** @name Operations */ /// @{
	/**
		Make an output serializer for the prop stream.
	*/
	OutputSerializer
	make_serializer() noexcept {
		return make_output_serializer(get_stream());
	}

	/**
		See IO::Datastore::acquire_output_stream().

		@pre Not acquired.
		@post Acquired.
	*/
	void
	acquire();

	/**
		Acquire non-datastore stream.

		@pre Not acquired.
		@post Acquired.
	*/
	void
	acquire_weak(
		std::ostream& stream
	) noexcept;

	/**
		See IO::Datastore::release_output_stream().

		@post Not acquired.
	*/
	void
	release();
/// @}
};

/** @} */ // end of doc-group prop
/** @} */ // end of doc-group io

} // namespace IO
} // namespace Hord

#endif // HORD_IO_PROPSTREAM_HPP_
