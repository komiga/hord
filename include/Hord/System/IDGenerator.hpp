/**
@file System/IDGenerator.hpp
@brief IDGenerator class.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
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
*/
class IDGenerator final {
private:
	std::mt19937 m_rng{std::mt19937::default_seed};

	IDGenerator(IDGenerator const&) = delete;
	IDGenerator& operator=(IDGenerator const&) = delete;

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	IDGenerator() /*noexcept*/;
	/** Move constructor. */
	IDGenerator(IDGenerator&&) noexcept;
	/** Destructor. */
	~IDGenerator() noexcept;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	IDGenerator& operator=(IDGenerator&&) noexcept;
/// @}

/** @name Operations */ /// @{
	/**
		Seed the generator.

		@param seed_value Seed value.
	*/
	void
	seed(
		int64_t const seed_value
	) noexcept;

	/**
		Generate an ID.

		@post
		@code
			generate() != Object::NULL_ID
		@endcode
		@returns The generated ID.
	*/
	Object::ID
	generate() noexcept;

	/**
		Generate unique ID within set.

		@tparam Set Set type. Must be an associative container
		with @c Object::ID as its key type.
		@returns The generated ID.
		@param set Set to generate within.
	*/
	template<
		typename Set
	>
	Object::ID
	generate_unique(
		Set const& set
	) noexcept {
		Object::ID id;
		do { id = m_rng(); }
		while (Object::NULL_ID == id || set.cend() != set.find(id));
		return id;
	}
/// @}
};

/** @} */ // end of doc-group driver
/** @} */ // end of doc-group system

} // namespace System
} // namespace Hord

#endif // HORD_SYSTEM_IDGENERATOR_HPP_