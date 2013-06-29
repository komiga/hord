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
#include "./Datastore.hpp"
#include "./Hive.hpp"
#include "./Rule.hpp"

#include <memory>

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
	using datastore_map_type
	= aux::unordered_map<
		HiveID,
		std::unique_ptr<Datastore>
	>;

	using rule_type_map_type
	= aux::unordered_map<
		RuleType,
		Rule::type_info const&
	>;

	using id_vector_type = aux::vector<HiveID>;

	IDGenerator& m_id_generator;
	rule_type_map_type m_rule_types{};
	datastore_map_type m_datastores{};
	id_vector_type m_hive_order{};

	Driver() = delete;
	Driver(Driver const&) = delete;
	Driver& operator=(Driver const&) = delete;
	Driver& operator=(Driver&&) = delete;

public:
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with ID generator.

		@warning All references must be valid for the lifetime
		of the driver.

		@param id_generator IDGenerator.
	*/
	explicit
	Driver(
		IDGenerator& id_generator
	) noexcept;

	/** Move constructor. */
	Driver(Driver&&);
	/** Destructor. */
	~Driver() noexcept;
/// @}

/** @name Properties */ /// @{
	/**
		Get IDGenerator.

		@returns The IDGenerator.
	*/
	IDGenerator&
	get_id_generator() noexcept {
		return m_id_generator;
	}
/// @}

/** @name Operations */ /// @{
	/**
		Register Rule type information.

		@remarks %Client-defined rules must be registered for them to
		be recognized during (de)serialization.

		@throws Error{ErrorCode::driver_rule_type_reserved}
		If @c type_info.type is reserved by StandardRuleTypes
		(including StandardRuleTypes::None).

		@throws Error{ErrorCode::driver_rule_type_zero_permitted_types}
		If @c type_info.permitted_types is @c 0.

		@throws Error{ErrorCode::driver_rule_type_shared}
		If a rule type @c type_info.type has already been registered.

		@param type_info Rule type information to register.
	*/
	void
	register_rule_type(
		Rule::type_info const& type_info
	);

	/**
		Placehold hive.

		@remarks %Hives are initially ordered in placehold order.

		@warning @c ErrorCode::driver_hive_root_shared is only caused
		by string comparison. It is possible for multiple placeheld
		hives to share the same actual directory, but the second one
		to attempt deserialization will fail
		with @c ErrorCode::driver_datastore_locked.

		@throws Error{ErrorCode::driver_hive_root_empty}
		If @c true==root_path.empty().

		@throws Error{ErrorCode::driver_hive_root_shared}
		If the root path has already been placeheld.

		@throws Error{ErrorCode::driver_datastore_construct_failed}
		If the datastore failed to construct (see
		Datastore::type_info::construct()).

		@post Emplaced hive with @a root_path
		and @c StorageState::placeholder.

		@returns The placeheld hive.
		@param type_info %Datastore type for the hive.
		@param root_path Root path.
	*/
	Hive const&
	placehold_hive(
		Datastore::type_info const& type_info,
		String root_path
	);

	/**
		Serialize objects.

		@param all Whether to serialize all objects, or only
		those that have @c StorageState::modified.
	*/
	void
	serialize(
		bool const all = false
	);
/// @}
};

/** @} */ // end of doc-group driver

} // namespace Hord

#endif // HORD_DRIVER_HPP_
