/**
@file Driver.hpp
@brief Driver.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_DRIVER_HPP_
#define HORD_DRIVER_HPP_

#include "./config.hpp"
#include "./common_types.hpp"
#include "./fwd.hpp"
#include "./aux.hpp"

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
public:
	/**
		Hive vector.
	*/
	typedef aux::vector<Hive> hive_vector_type;

private:
	Serializer& m_serializer;
	IDGenerator& m_id_generator;
	hive_vector_type m_hives{};

	Driver()=delete;
	Driver(Driver const&)=delete;
	Driver& operator=(Driver const&)=delete;
	Driver& operator=(Driver&&)=delete;

public:
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with Serializer and IDGenerator.
		@param serializer Serializer.
		@param id_generator IDGenerator.
	*/
	Driver(Serializer& serializer, IDGenerator& id_generator)
		: m_serializer{serializer}
		, m_id_generator{id_generator}
	{}
	/** Move constructor. */
	Driver(Driver&&)=default;
	/** Destructor. */
	~Driver()=default;
/// @}

/** @name Properties */ /// @{
	/**
		Get Serializer.
		@returns The Serializer.
	*/
	Serializer& get_serializer() noexcept { return m_serializer; }

	/**
		Get IDGenerator.
		@returns The IDGenerator.
	*/
	IDGenerator& get_id_generator() noexcept { return m_id_generator; }

	/**
		Get Hive collection.
		@returns The Hive collection.
	*/
	hive_vector_type const& get_hives() const noexcept { return m_hives; }
/// @}

/** @name Operations */ /// @{
	/**
		Placehold Hive.
		@warning @c ErrorCode::driver_hive_root_shared is only caused by string comparison.
		It is possible for multiple placeheld hives to share the same actual directory, but the second one to attempt deserialization will fail with @c ErrorCode::driver_hive_locked.
		@throws Error @c ErrorCode::driver_hive_root_empty: If @c true==root.empty().
		@throws Error @c ErrorCode::driver_hive_root_shared: If a Hive with root path @a root has already been placeheld.
		@post Emplaced Hive with @a root and @c StorageState::placeholder.
		@returns The placeheld Hive.
		@param root Root path.
		@sa get_hives() const
	*/
	Hive& placehold_hive(String root);
	/**
		Serialize objects.
		@param all Whether to serialize all objects, or only those that have @c StorageState::modified.
	*/
	void serialize(bool const all=false);
/// @}
};

/** @} */ // end of doc-group driver

} // namespace Hord

#endif // HORD_DRIVER_HPP_
