/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Generic schema unit class.
@ingroup schema
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Schema/Defs.hpp>
#include <Hord/Schema/Unit.hpp>

namespace Hord {
namespace Schema {

// Forward declarations
class UnitGeneric;

/**
	@addtogroup schema
	@{
*/

/**
	Generic schema unit class.
*/
class UnitGeneric final
	: public Schema::Unit
{
private:
	using base = Schema::Unit;

	static Object::UPtr
	construct(
		Object::ID const id,
		Object::ID const parent
	) noexcept;

public:
	/**
		Type info.
	*/
	static constexpr Object::TypeInfo const
	info{
		"Hord.Schema.Generic",
		Schema::Type{Schema::UnitType::Generic},
		{true, false},
		UnitGeneric::construct
	};

private:
	UnitGeneric() = delete;
	UnitGeneric(UnitGeneric const&) = delete;
	UnitGeneric& operator=(UnitGeneric const&) = delete;

// Schema::Unit implementation
	void
	deserialize_prop_auxiliary(
		IO::InputPropStream& prop_stream,
		InputSerializer& ser
	) override;

	void
	serialize_prop_auxiliary(
		IO::OutputPropStream& prop_stream,
		OutputSerializer& ser
	) const override;

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~UnitGeneric() noexcept override;

	/** Move constructor. */
	UnitGeneric(UnitGeneric&&);
	/** Move assignment operator. */
	UnitGeneric& operator=(UnitGeneric&&);

private:
	UnitGeneric(
		Schema::ID const id,
		Object::ID const parent
	) noexcept;
/// @}
};

/** @} */ // end of doc-group schema

} // namespace Schema

template struct Schema::Unit::ensure_traits<Schema::UnitGeneric>;

} // namespace Hord
