/**
@file Driver.hpp
@brief Driver.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_DRIVER_HPP_
#define HORD_DRIVER_HPP_

#include "./config.hpp"
#include "./common_types.hpp"
#include "./aux.hpp"
#include "./String.hpp"
#include "./IDGenerator.hpp"
#include "./Serializer.hpp"
#include "./Hive.hpp"

namespace Hord {

// Forward declarations
class Driver;

/**
	@addtogroup driver
	@{
*/

/**
	Driver.
*/
class Driver final {
private:
	typedef aux::unordered_map<HiveID, Hive> hive_map_type;
	typedef aux::vector<HiveID> id_vector_type;

	Serializer& m_serializer;
	IDGenerator& m_id_generator;
	hive_map_type m_hives{};
	id_vector_type m_hive_order{};

	Driver()=delete;
	Driver(Driver const&)=delete;
	Driver& operator=(Driver const&)=delete;

public:
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with Serializer and IDGenerator.

		@warning Both references must be valid for the lifetime
		of the driver.

		@param serializer Serializer.
		@param id_generator IDGenerator.
	*/
	Driver(Serializer& serializer, IDGenerator& id_generator) noexcept;
	/** Move constructor. */
	Driver(Driver&&)=default;
	/** Destructor. */
	~Driver()=default;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	Driver& operator=(Driver&&)=default;
/// @}

/** @name Properties */ /// @{
	/**
		Get Serializer.
		@returns The Serializer.
	*/
	Serializer& get_serializer() noexcept
		{ return m_serializer; }

	/**
		Get IDGenerator.
		@returns The IDGenerator.
	*/
	IDGenerator& get_id_generator() noexcept
		{ return m_id_generator; }
/// @}

/** @name Operations */ /// @{
	/**
		Placehold Hive.

		@warning @c ErrorCode::driver_hive_root_shared is only caused
		by string comparison. It is possible for multiple placeheld
		hives to share the same actual directory, but the second one
		to attempt deserialization will fail
		with @c ErrorCode::driver_hive_locked.

		@throws Error{ErrorCode::driver_hive_root_empty}
		If @c true==root.empty().

		@throws Error{ErrorCode::driver_hive_root_shared}
		If a Hive with root path @a root has already been placeheld.

		@post Emplaced Hive with @a root and @c StorageState::placeholder.

		@returns The placeheld Hive.
		@param root Root path.
	*/
	Hive& placehold_hive(String root);

	/**
		Serialize objects.

		@param all Whether to serialize all objects, or only
		those that have @c StorageState::modified.
	*/
	void serialize(bool const all=false);
/// @}
};

/** @} */ // end of doc-group driver

} // namespace Hord

#endif // HORD_DRIVER_HPP_
