/**
@file PropStream.hpp
@brief PropStream classes.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_PROPSTREAM_HPP_
#define HORD_PROPSTREAM_HPP_

#include "./config.hpp"
#include "./common_enums.hpp"
#include "./Prop.hpp"
#include "./Datastore.hpp"

#include <iosfwd>

namespace Hord {

// Forward declarations
class InputPropStream;
class OutputPropStream;

/**
	@addtogroup serialization
	@{
*/
/**
	@addtogroup prop
	@{
*/

/**
	Base input prop stream.

	@remarks This can be used as a generic prop stream.
*/
class InputPropStream final {
private:
	Datastore& m_datastore;
	PropInfo m_info;
	std::istream* m_stream;

	InputPropStream()=delete;
	InputPropStream(InputPropStream const&)=delete;
	InputPropStream& operator=(InputPropStream const&)=delete;
	InputPropStream& operator=(InputPropStream&&)=delete;

	void acquire();
	void release();

public:
	/**
		Acquirement constructor.

		@throws Error{..}
		See Datastore::acquire_input_stream().

		@param datastore %Datastore.
		@param info Prop info.
	*/
	InputPropStream(
		Datastore& datastore,
		PropInfo info
	);
	/** Move constructor. */
	InputPropStream(InputPropStream&&) noexcept;
	/**
		Destructor.

		Automatically releases stream if it is valid.

		@throws Error{..}
		See Datastore::release_input_stream().
	*/
	~InputPropStream();
/// @}

public:
/** @name Properties */ /// @{
	/**
		Get datastore.
		@returns %Datastore.
	*/
	Datastore& get_datastore() noexcept
		{ return m_datastore; }

	/**
		Get info.
		@returns Info.
	*/
	PropInfo const& get_info() const noexcept
		{ return m_info; }

	/**
		Get raw input stream.

		@warning If this happens to be called on a destroyed prop
		stream, the return value will be a null reference and
		terrific things will happen.

		@returns Raw input stream.
	*/
	std::istream& get_stream();
/// @}
};

/**
	Base output prop stream.
*/
class OutputPropStream final {
private:
	Datastore& m_datastore;
	PropInfo m_info;
	std::ostream* m_stream;

	OutputPropStream()=delete;
	OutputPropStream(OutputPropStream const&)=delete;
	OutputPropStream& operator=(OutputPropStream const&)=delete;
	OutputPropStream& operator=(OutputPropStream&&)=delete;

	void acquire();
	void release();

public:
	/**
		Acquirement constructor.

		@throws Error{..}
		See Datastore::acquire_output_stream().

		@param datastore %Datastore.
		@param info Prop info.
	*/
	OutputPropStream(
		Datastore& datastore,
		PropInfo info
	);
	/** Move constructor. */
	OutputPropStream(OutputPropStream&&) noexcept;
	/**
		Destructor.

		Automatically releases stream if it is valid.

		@throws Error{..}
		See Datastore::release_output_stream().
	*/
	~OutputPropStream();
/// @}

public:
/** @name Properties */ /// @{
	/**
		Get datastore.
		@returns %Datastore.
	*/
	Datastore& get_datastore() noexcept
		{ return m_datastore; }

	/**
		Get info.
		@returns Info.
	*/
	PropInfo const& get_info() const noexcept
		{ return m_info; }

	/**
		Get raw output stream.

		@warning If this happens to be called on a destroyed prop
		stream, the return value will be a null reference and
		terrific things will happen.

		@returns Raw output stream.
	*/
	std::ostream& get_stream();
/// @}
};

/** @} */ // end of doc-group prop
/** @} */ // end of doc-group serialization

} // namespace Hord

#endif // HORD_PROPSTREAM_HPP_
