/**
@file System/IDGenerator.hpp
@brief IDGenerator class.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_SYSTEM_IDGENERATOR_HPP_
#define HORD_SYSTEM_IDGENERATOR_HPP_

#include <Hord/config.hpp>
#include <Hord/Object/Defs.hpp>

#include <random>

namespace Hord {
namespace System {

// Forward declarations
class IDGenerator;

/**
	@addtogroup system
	@{
*/
/**
	@addtogroup driver
	@{
*/

/**
	Object ID generator.

	@warning Objects of this class are rather large due to the
	internal @c std::mt19937 object, which maintains a hefty state.
*/
class IDGenerator final {
private:
	std::mt19937 m_rng{std::mt19937::default_seed};

	IDGenerator(IDGenerator const&) = delete;
	IDGenerator& operator=(IDGenerator const&) = delete;

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~IDGenerator() noexcept;

	/** Default constructor. */
	IDGenerator() /*noexcept*/;
	/** Move constructor. */
	IDGenerator(IDGenerator&&) noexcept;
	/** Move assignment operator. */
	IDGenerator& operator=(IDGenerator&&) noexcept;
/// @}

/** @name Operations */ /// @{
	/**
		Seed the generator.
	*/
	void
	seed(
		std::int64_t const seed_value
	) noexcept;

	/**
		Generate an ID.

		@post
		@code
			generate() != Object::ID_NULL
		@endcode
	*/
	template<
		Object::BaseType const B = Object::BaseType::null
	>
	Object::GenID<B>
	generate() noexcept {
		Object::GenID<B> id{};
		do { id.assign(m_rng()); }
		while (id.is_reserved());
		return id;
	}

	/**
		Generate unique ID within set.

		@tparam Set Set type. Must be an associative container
		with @c Object::ID as its key type.
		@returns The generated ID.
	*/
	template<typename Set>
	auto
	generate_unique(
		Set const& set
	) noexcept -> typename std::remove_const<typename Set::key_type>::type {
		using id_type = typename std::remove_const<
			typename Set::key_type
		>::type;
		static_assert(
			Object::is_genid<id_type>::value,
			"Set type must have an Object::GenID as its key type"
		);
		id_type id{};
		do { id.assign(m_rng()); }
		while (id.is_reserved() || set.cend() != set.find(id));
		return id;
	}
/// @}
};

/** @} */ // end of doc-group driver
/** @} */ // end of doc-group system

} // namespace System
} // namespace Hord

#endif // HORD_SYSTEM_IDGENERATOR_HPP_
