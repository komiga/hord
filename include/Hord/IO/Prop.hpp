/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Prop structures.
@ingroup prop
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/Object/Defs.hpp>

namespace Hord {
namespace IO {

// Forward declarations
struct PropInfo;

/**
	@addtogroup prop
	@{
*/

/**
	Prop info.

	@sa IO::PropStream
*/
struct PropInfo final {
public:
/** @name Properties */ /// @{
	/** Object ID. */
	Object::ID object_id;

	/** Object type. */
	Object::Type object_type;

	/** Prop type. */
	IO::PropType prop_type;
/// @}

/** @name Special member functions */ /// @{
	/** Destructor. */
	~PropInfo() noexcept = default;

	/** Copy constructor. */
	PropInfo(PropInfo const&) noexcept = default;
	/** Move constructor. */
	PropInfo(PropInfo&&) noexcept = default;
	/** Copy assignment operator. */
	PropInfo& operator=(PropInfo const&) noexcept = default;
	/** Move assignment operator. */
	PropInfo& operator=(PropInfo&&) noexcept = default;

	/**
		Constructor with properties.
	*/
	constexpr
	PropInfo(
		Object::ID const object_id,
		Object::Type const object_type,
		IO::PropType const prop_type
	) noexcept
		: object_id(object_id)
		, object_type(object_type)
		, prop_type(prop_type)
	{}

	/**
		Constructor with object.

		@post @code
			this->object_id == object.id() &&
			this->object_type == object.type()
		@endcode
	*/
	PropInfo(
		Object::Unit const& object,
		IO::PropType const prop_type
	) noexcept;
/// @}
};

/** @} */ // end of doc-group prop

} // namespace IO
} // namespace Hord
