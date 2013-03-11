/**
@file IDGenerator.hpp
@brief IDGenerator class.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_IDGENERATOR_HPP_
#define HORD_IDGENERATOR_HPP_

#include "./config.hpp"
#include "./common_types.hpp"

namespace Hord {

// Forward declarations
class IDGenerator;

/**
	@addtogroup driver
	@{
*/

/**
	ObjectID generator.
*/
class IDGenerator {
private:
	IDGenerator(IDGenerator const&)=delete;
	IDGenerator& operator=(IDGenerator const&)=delete;

	/**
		seed() implementation.

		@remark If called from Driver, @a seed_value will be a growing
		value, based on time.
	*/
	virtual void seed_impl(int64_t seed_value) noexcept=0;
	/**
		generate() implementation.

		@post Return value must not be equal to @c OBJECT_NULL.
	*/
	virtual ObjectID generate_impl() noexcept=0;

public:
/** @name Constructors and destructor */ /// @{
	/** Default constructor. */
	IDGenerator()=default;
	/** Move constructor. */
	IDGenerator(IDGenerator&&)=default;
	/** Destructor. */
	inline virtual ~IDGenerator()=0;
/// @}

/** @name Operators */ /// @{
	/** Move assignment operator. */
	IDGenerator& operator=(IDGenerator&&)=default;
/// @}

/** @name Operations */ /// @{
	/**
		Seed the generator.
		@param seed_value Seed value.
	*/
	void seed(int64_t const seed_value) noexcept
		{ seed_impl(seed_value); }
	/**
		Generate an ID.

		@post
		@code
			generate()!=OBJECT_NULL
		@endcode
		@returns The generated ID.
	*/
	ObjectID generate() noexcept
		{ return generate_impl(); }
	/**
		Generate unique ID within set.

		@tparam Set Set type. Must be an associative container
		with ObjectID as its key type.
		@returns The generated ID.
		@param set Set to generate within.
	*/
	template<typename Set>
	ObjectID generate_unique(Set const& set) noexcept {
		ObjectID id;
		do { id=generate(); } while (set.cend()!=set.find(id));
		return id;
	}
/// @}
};
inline IDGenerator::~IDGenerator()=default;

/** @} */ // end of doc-group driver

} // namespace Hord

#endif // HORD_IDGENERATOR_HPP_
