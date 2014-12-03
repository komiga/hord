/**
@file
@brief Basic anchor unit class.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Anchor/Defs.hpp>
#include <Hord/Anchor/Unit.hpp>

namespace Hord {
namespace Anchor {

// Forward declarations
class UnitBasic;

/**
	@addtogroup object
	@{
*/
/**
	@addtogroup anchor
	@{
*/

/**
	Basic anchor unit class.
*/
class UnitBasic final
	: public Anchor::Unit
{
private:
	using base = Anchor::Unit;

	static Object::UPtr
	construct(
		Object::ID const id,
		Object::ID const parent
	) noexcept;

public:
	/**
		Type info.
	*/
	static constexpr Object::type_info const
	info{
		"Hord.Anchor.Basic",
		Anchor::Type{Anchor::UnitType::basic},
		{Anchor::SUPPLIED_PROPS},
		UnitBasic::construct
	};

private:
	UnitBasic() = delete;
	UnitBasic(UnitBasic const&) = delete;
	UnitBasic& operator=(UnitBasic const&) = delete;

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~UnitBasic() noexcept override;

	/** Move constructor. */
	UnitBasic(UnitBasic&&);
	/** Move assignment operator. */
	UnitBasic& operator=(UnitBasic&&);

private:
	UnitBasic(
		Anchor::ID const id,
		Object::ID const parent
	) noexcept;
/// @}
};

/** @} */ // end of doc-group anchor
/** @} */ // end of doc-group object

} // namespace Anchor

template struct Anchor::Unit::ensure_traits<Anchor::UnitBasic>;

} // namespace Hord
