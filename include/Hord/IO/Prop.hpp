/**
@file IO/Prop.hpp
@brief Prop structures.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_IO_PROP_HPP_
#define HORD_IO_PROP_HPP_

#include <Hord/config.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/Object/Defs.hpp>

namespace Hord {
namespace IO {

// Forward declarations
struct PropInfo;

/**
	@addtogroup io
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
	Object::ID object_id;
	/** Prop type. */
	IO::PropType prop_type;
};

/** @} */ // end of doc-group prop
/** @} */ // end of doc-group io

} // namespace IO
} // namespace Hord

#endif // HORD_IO_PROP_HPP_
