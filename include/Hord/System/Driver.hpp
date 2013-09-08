/**
@file System/Driver.hpp
@brief Driver.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_SYSTEM_DRIVER_HPP_
#define HORD_SYSTEM_DRIVER_HPP_

#include <Hord/config.hpp>
#include <Hord/cc_unique_ptr.hpp>
#include <Hord/aux.hpp>
#include <Hord/String.hpp>
#include <Hord/System/IDGenerator.hpp>
#include <Hord/IO/Datastore.hpp>
#include <Hord/Rule/Defs.hpp>
#include <Hord/Hive/Defs.hpp>
#include <Hord/Hive/Unit.hpp>

#include <utility>
#include <memory>

namespace Hord {
namespace System {

// Forward declarations
class Driver;

/**
	@addtogroup system
	@{
*/
/**
	@addtogroup driver
	@{
*/

/**
	Driver.
*/
class Driver final {
private:
	using rule_type_map_type
	= aux::unordered_map<
		Rule::Type,
		Rule::type_info const&
	>;

	using hive_map_type
	= aux::unordered_map<
		Hive::ID,
		std::pair<
			cc_unique_ptr<IO::Datastore>,
			Hive::Unit
		>
	>;

	using hive_id_vector_type = aux::vector<Hive::ID>;

	System::IDGenerator m_id_generator{};
	rule_type_map_type m_rule_types{};
	hive_map_type m_hives{};
	hive_id_vector_type m_hive_order{};

	Driver(Driver const&) = delete;
	Driver& operator=(Driver const&) = delete;
	Driver& operator=(Driver&&) = delete;

public:
/** @name Constructors and destructor */ /// @{
	/**
		Default constructor.
	*/
	Driver() /*noexcept*/;
	/** Move constructor. */
	Driver(Driver&&);
	/** Destructor. */
	~Driver() noexcept;
/// @}

/** @name Operations */ /// @{
	/**
		Register Rule type information.

		@remarks %Client-defined rules must be registered for them to
		be recognized during (de)serialization.

		@throws Error{ErrorCode::driver_rule_type_reserved}
		If @c type_info.type is reserved by Rule::StandardTypes
		(including Rule::StandardTypes::None).

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
		IO::Datastore::type_info::construct()).

		@post Emplaced hive with @a root_path
		and @c IO::StorageState::placeholder.

		@returns The placeheld hive.
		@param type_info %Datastore type for the hive.
		@param root_path Root path.
	*/
	Hive::Unit const&
	placehold_hive(
		IO::Datastore::type_info const& type_info,
		String root_path
	);

	/**
		Serialize objects.

		@param all Whether to serialize all objects, or only
		those that have @c IO::StorageState::modified.
	*/
	void
	serialize(
		bool const all = false
	);
/// @}
};

/** @} */ // end of doc-group driver
/** @} */ // end of doc-group system

} // namespace System
} // namespace Hord

#endif // HORD_SYSTEM_DRIVER_HPP_
