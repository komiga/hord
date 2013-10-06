/**
@file IO/PropStream.hpp
@brief PropStream classes.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_IO_PROPSTREAM_HPP_
#define HORD_IO_PROPSTREAM_HPP_

#include <Hord/config.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/Prop.hpp>
#include <Hord/IO/Datastore.hpp>

#include <iosfwd>

namespace Hord {
namespace IO {

// Forward declarations
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
*/
class PropStream {
protected:
	/** %Datastore. */
	IO::Datastore& m_datastore;

	/** Prop info. */
	IO::PropInfo const m_info;

private:
	PropStream() = delete;
	PropStream(PropStream const&) = delete;
	PropStream& operator=(PropStream const&) = delete;
	PropStream& operator=(PropStream&&) = delete;

public:
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with datastore and prop info.

		@param datastore %Datastore.
		@param info Prop info.
	*/
	PropStream(
		IO::Datastore& datastore,
		IO::PropInfo info
	)
		: m_datastore(datastore)
		, m_info(std::move(info))
	{}

	/** Move constructor. */
	PropStream(PropStream&&) noexcept = default;

	/**
		Destructor.
	*/
	~PropStream() noexcept = default;
/// @}

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
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with datastore and prop info.

		@param datastore %Datastore.
		@param info Prop info.
	*/
	InputPropStream(
		IO::Datastore& datastore,
		IO::PropInfo info
	);

	/** Move constructor. */
	InputPropStream(InputPropStream&&) noexcept;

	/**
		Destructor.
	*/
	~InputPropStream() noexcept;
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
		See IO::Datastore::acquire_input_stream().
	*/
	void
	acquire();

	/**
		See IO::Datastore::release_input_stream().
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
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with datastore and prop info.

		@param datastore %Datastore.
		@param info Prop info.
	*/
	OutputPropStream(
		IO::Datastore& datastore,
		IO::PropInfo info
	);

	/** Move constructor. */
	OutputPropStream(OutputPropStream&&) noexcept;

	/**
		Destructor.
	*/
	~OutputPropStream() noexcept;
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
		See IO::Datastore::acquire_output_stream().
	*/
	void
	acquire();

	/**
		See IO::Datastore::release_output_stream().
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
