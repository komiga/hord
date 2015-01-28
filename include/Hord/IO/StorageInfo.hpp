/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief %Object storage information.
@ingroup io
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/serialization.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/PropStateStore.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Object/Unit.hpp>

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

/** @name Special member functions */ /// @{
	/** Destructor. */
	~StorageInfo() noexcept = default;

	/** Copy constructor. */
	StorageInfo(StorageInfo const&) noexcept = default;
	/** Move constructor. */
	StorageInfo(StorageInfo&&) noexcept = default;
	/** Copy assignment operator. */
	StorageInfo& operator=(StorageInfo const&) = default;
	/** Move assignment operator. */
	StorageInfo& operator=(StorageInfo&&) noexcept = default;

	/**
		Constructor with properties.
	*/
	constexpr
	StorageInfo(
		Object::ID const object_id,
		Object::Type const object_type,
		IO::PropStateStore const& prop_storage,
		IO::Linkage const linkage
	) noexcept
		: object_id(object_id)
		, object_type(object_type)
		, prop_storage(prop_storage)
		, linkage(linkage)
	{}

	/**
		Constructor with object and linkage.

		@post @code
			this->object_id == object.id() &&
			this->object_type == object.type() &&
			this->prop_storage == object.type_info().props &&
		@endcode
	*/
	StorageInfo(
		Object::Unit const& object,
		IO::Linkage const linkage
	) noexcept
		: StorageInfo(
			object.id(),
			object.type(),
			object.type_info().props,
			linkage
		)
	{}
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
