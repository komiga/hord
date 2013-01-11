/**
@file common_types.hpp
@brief Common types.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_COMMON_TYPES_HPP_
#define HORD_COMMON_TYPES_HPP_

#include "./config.hpp"

namespace Hord {

/** Generic object ID. */
typedef uint32_t ObjectID;

/** Node ID. */
typedef ObjectID NodeID;
/** Rule ID. */
typedef ObjectID RuleID;

/** MetaField type. */
typedef uint8_t MetaFieldType;
/** Rule type. */
typedef uint32_t RuleType;

} // namespace Hord

#endif // HORD_COMMON_TYPES_HPP_
