/**
@file IO/StorageInfo.hpp
@brief %Object storage information.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_IO_STORAGEINFO_HPP_
#define HORD_IO_STORAGEINFO_HPP_

#include <Hord/config.hpp>
#include <Hord/serialization.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/IO/Defs.hpp>

namespace Hord {
namespace IO {

// Forward declarations
struct StorageInfo;

/**
	@addtogroup io
	@{
*/

/**
	%Object storage information.

	@note The @c prop_storage member variable shows the state of the
	object's props. If the datastore has storage for the prop, its
	state is set to IO::PropState::original. Otherwise, it is left
	uninitialized. Therefore, the storage of a prop can be checked
	with IO::PropStateStore::is_initialized().
*/
struct StorageInfo final {
public:
/** @name Properties */ /// @{
	/** %Object %ID. */
	Object::ID object_id;

	/** %Object type. */
	Object::Type object_type;

	/** %Prop storage states. */
	IO::PropStateStore prop_storage;

	/** %Object linkage. */
	IO::Linkage linkage;
/// @}
};

/**
	Serialize StorageInfo.
*/
template<class Ser>
inline ser_result_type
serialize(
	ser_tag_serialize,
	Ser& ser,
	IO::StorageInfo& value
) {
	auto& self = const_safe<Ser>(value);
	ser(
		self.object_id,
		self.object_type,
		self.prop_storage,
		self.linkage
	);
}

/** @} */ // end of doc-group io

} // namespace IO
} // namespace Hord

#endif // HORD_IO_STORAGEINFO_HPP_

