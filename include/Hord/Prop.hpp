/**
@file Prop.hpp
@brief Prop structures.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_PROP_HPP_
#define HORD_PROP_HPP_

#include "./config.hpp"
#include "./common_enums.hpp"

namespace Hord {

// Forward declarations
struct PropInfo;

/**
	@addtogroup serialization
	@{
*/
/**
	@addtogroup prop
	@{
*/

/**
	Prop info.
*/
struct PropInfo final {
public:
	/** %Object ID. */
	ObjectID object_id;
	/** Prop type. */
	PropType prop_type;
};

/** @} */ // end of doc-group prop
/** @} */ // end of doc-group serialization

} // namespace Hord

#endif // HORD_PROP_HPP_
