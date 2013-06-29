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
class IDGenerator {
private:
	IDGenerator(IDGenerator const&) = delete;
	IDGenerator& operator=(IDGenerator const&) = delete;

protected:
/** @name Implementation */ /// @{
	/**
		seed() implementation.

		@remarks If called from Driver, @a seed_value will be a
		growing value, based on time.
	*/
	virtual void
	seed_impl(
		int64_t seed_value
	) noexcept = 0;

	/**
		generate() implementation.

		@post Return value must not be equal to @c Object::NULL_ID.
	*/
	virtual Object::ID
	generate_impl() noexcept = 0;
/// @}

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	IDGenerator() noexcept;
	/** Move constructor. */
	IDGenerator(IDGenerator&&) noexcept;
	/** Destructor. */
	virtual
	~IDGenerator() noexcept = 0;
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
	) noexcept {
		seed_impl(seed_value);
	}

	/**
		Generate an ID.

		@post
		@code
			generate() != Object::NULL_ID
		@endcode
		@returns The generated ID.
	*/
	Object::ID
	generate() noexcept {
		return generate_impl();
	}

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
		do { id = generate(); }
		while (set.cend() != set.find(id));
		return id;
	}
/// @}
};

/** @} */ // end of doc-group driver
/** @} */ // end of doc-group system

} // namespace System
} // namespace Hord

#endif // HORD_SYSTEM_IDGENERATOR_HPP_
