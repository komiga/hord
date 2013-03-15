/**
@file Serializer.hpp
@brief Serializer class.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_SERIALIZER_HPP_
#define HORD_SERIALIZER_HPP_

#include "./config.hpp"
#include "./common_enums.hpp"
#include "./String.hpp"
#include "./Object.hpp"

namespace Hord {

// Forward declarations
class Serializer;

/**
	@addtogroup serialization
	@{
*/

/**
	Base object serializer.
*/
class Serializer {
private:
	Serializer(Serializer const&)=delete;
	Serializer& operator=(Serializer const&)=delete;

protected:
/** @name Implementation */ /// @{
	/**
		deserialize_object() implementation.
	*/
	virtual void deserialize_object_impl(
		Object& object,
		SerializationFlags flags
	)=0;
	/**
		serialize_object() implementation.
	*/
	virtual void serialize_object_impl(
		Object const& object,
		SerializationFlags flags
	)=0;
/// @}

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	Serializer()=default;
	/** Move constructor. */
	Serializer(Serializer&&)=default;
	/** Destructor. */
	inline virtual ~Serializer()=0;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Serializer& operator=(Serializer&&)=default;
/// @}

/** @name Operations */ /// @{
	/**
		Deserialize object.

		@note @c SerializationFlags::identity is implicit.

		@throws Error{ErrorCode::serialization_improper_state}
		If @a object does not have @c StorageState::placeholder.

		@throws Error{ErrorCode::serialization_access}
		If data for @a object could not be accessed.

		@param object Object to deserialize.
		@param flags Serialization flags.
	*/
	void deserialize_object(
		Object& object,
		SerializationFlags const flags
	) { deserialize_object_impl(object, flags); }

	/**
		Serialize object.

		@throws Error{ErrorCode::serialization_improper_state}
		If @a object does not have @c StorageState::modified.

		@throws Error{ErrorCode::serialization_access}
		If data for @a object could not be accessed.

		@param object Object to serialize.
		@param flags Serialization flags.
	*/
	void serialize_object(
		Object const& object,
		SerializationFlags const flags
	) { serialize_object_impl(object, flags); }
/// @}
};
inline Serializer::~Serializer()=default;

/** @} */ // end of doc-group serialization

} // namespace Hord

#endif // HORD_SERIALIZER_HPP_
