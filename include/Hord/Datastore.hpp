/**
@file Datastore.hpp
@brief Datastore class.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_DATASTORE_HPP_
#define HORD_DATASTORE_HPP_

#include "./config.hpp"
#include "./common_enums.hpp"
#include "./Prop.hpp"
#include "./Hive.hpp"

#include <functional>
#include <iosfwd>

// FIXME: libstdc++ 4.6.3 does not use the noexcept specifier
// on reference_wrapper ctors.

namespace Hord {

// Forward declarations
class Datastore;

/**
	@addtogroup serialization
	@{
*/
/**
	@addtogroup datastore
	@{
*/

/**
	Base datastore.

	This is the data interface for hives.

	@remarks The datastore's associated hive should not be used by
	an implementation to lookup objects within the datastore. The
	hive is not guaranteed to contain the entire idset at runtime.

	@remarks The associated hive will always have a valid ID, but
	note that the hive's ID is <strong>not</strong> within the ID
	space of its children -- see @ref hive.
*/
class Datastore {
private:
	std::reference_wrapper<Hive const> m_hive;

	Datastore()=delete;
	Datastore(Datastore const&)=delete;
	Datastore& operator=(Datastore const&)=delete;

protected:
/** @name Implementation */ /// @{
	/**
		is_open() implementation.
	*/
	virtual bool is_open_impl() const noexcept=0;

	/**
		Acquire raw stream implementation.
		@{
	*/
	virtual std::istream& acquire_input_stream_impl(
		PropInfo const& prop_info
	)=0;
	virtual std::ostream& acquire_output_stream_impl(
		PropInfo const& prop_info
	)=0;
	/** @} */

	/**
		Release raw stream implementation.
		@{
	*/
	virtual void release_input_stream_impl(
		PropInfo const& prop_info
	)=0;
	virtual void release_output_stream_impl(
		PropInfo const& prop_info
	)=0;
	/** @} */
/// @}

protected:
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with hive.

		@param hive Associated hive.
	*/
	Datastore(Hive const& hive) noexcept;
	/** Move constructor. */
	Datastore(Datastore&&);
public:
	/** Destructor. */
	virtual ~Datastore()=0;
/// @}

protected:
/** @name Operators */ /// @{
	/** Move assignment operator. */
	Datastore& operator=(Datastore&&);
/// @}

public:
/** @name Properties */ /// @{
	/**
		Get associated hive.
		@returns Associated hive.
	*/
	Hive const& get_hive() const noexcept
		{ return m_hive.get(); }

	/**
		Check if the datastore is open.
		@returns
		- @c true if the datastore is open;
		- @c false if it is closed.
	*/
	bool is_open() const noexcept {
		return is_open_impl();
	}
/// @}

/** @name Operations */ /// @{
	/**
		Open the datastore.

		@throws Error{ErrorCode::datastore_already_opened}
		If the datastore is already open.
	*/
	void open();
	/**
		Close the datastore.
	*/
	void close();

	/**
		Acquire raw stream for prop.

		@throws Error{ErrorCode::datastore_closed}
		If the datastore is closed.

		@throws Error{ErrorCode::datastore_object_not_found}
		If @c prop_info.object_id does not exist in the datastore.

		@throws Error{ErrorCode::datastore_prop_unsupplied}
		If the object for @c prop_info.object_id does not supply
		the requested prop.

		@throws Error{ErrorCode::datastore_locked}
		If the datastore is locked (including implementation-defined
		reasons). If <strong>any prop stream</strong> is active, the
		entire datastore is locked.

		@throws Error{..}
		<em>Implementation-defined exceptions.</em>

		@returns Raw prop stream.
		@param prop_info Prop info.

		@sa PropInfo,
			InputPropStream,
			OutputPropStream
		@{
	*/
	std::istream& acquire_input_stream(PropInfo const& prop_info);
	std::ostream& acquire_output_stream(PropInfo const& prop_info);
	/** @} */

	/**
		Release raw stream for prop.

		@throws Error{ErrorCode::datastore_closed}
		If the datastore is closed.

		@throws Error{ErrorCode::datastore_object_not_found}
		If @c prop_info.object_id does not exist in the datastore.

		@throws Error{ErrorCode::datastore_prop_unsupplied}
		If the object for @c prop_info.object_id does not supply
		the requested prop.

		@throws Error{ErrorCode::datastore_prop_not_locked}
		If either @a prop_info does not match the currently locked
		prop stream or there is no currently locked prop stream.

		@throws Error{..}
		<em>Implementation-defined exceptions.</em>

		@param prop_info Prop info.

		@sa PropInfo,
			InputPropStream,
			OutputPropStream
		@{
	*/
	void release_input_stream(PropInfo const& prop_info);
	void release_output_stream(PropInfo const& prop_info);
	/** @} */
/// @}
};

/** @} */ // end of doc-group datastore
/** @} */ // end of doc-group serialization

} // namespace Hord

#endif // HORD_DATASTORE_HPP_
