/**
@file
@brief Utilities.
@ingroup utility

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/String.hpp>

#include <am/hash/common.hpp>
#include <am/hash/fnv.hpp>

#include <duct/utility.hpp>

namespace Hord {

// Forward declarations

/**
	@addtogroup utility
	@{
*/

using duct::bit;
using duct::make_const;
using duct::enum_cast;
using duct::enum_bitand;
using duct::enum_combine;
using duct::min_ce;
using duct::max_ce;

/** @cond INTERNAL */
using HashImpl = am::hash::fnv1a<am::hash::HashLength::HL64>;
/** @endcond */ // INTERNAL

/**
	Hash value type.
*/
using HashValue = HashImpl::hash_type;

enum : HashValue {
	/** Empty hash value. */
	HASH_EMPTY = HashValue{0}
};

/**
	Hash combiner.
*/
using HashCombiner = am::hash::fnv1a_combiner<am::hash::HashLength::HL64>;

/** @cond INTERNAL */
/*namespace {
	enum : HashImpl::seed_type {
		HASH_SEED = 0xAD756E1B,
	};
}*/ // anonymous namespace
/** @endcond */ // INTERNAL

/**
	Calculate hash value of a byte sequence.
*/
inline HashValue
hash(
	char const* const data,
	unsigned const size
) noexcept {
	return am::hash::calc<HashImpl>(data, size);
}

/**
	Calculate hash value of a string.
*/
inline HashValue
hash_string(
	String const& string
) noexcept {
	return am::hash::calc_string<HashImpl>(string);
}

/**
	Calculate hash value of a byte sequence (constexpr).
*/
inline constexpr HashValue
hash_ce(
	char const* const data,
	unsigned const size
) noexcept {
	return am::hash::calc_ce<HashImpl>(data, size);
}

/**
	Hash UDL.
*/
inline constexpr HashValue
operator"" _hash(
	char const* const data,
	std::size_t const size
) {
	return hash_ce(data, size);
}

/** @} */ // end of doc-group utility

} // namespace Hord
