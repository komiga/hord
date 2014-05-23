/**
@file Hive/UnitBasic.hpp
@brief Hive basic unit class.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_HIVE_UNITBASIC_HPP_
#define HORD_HIVE_UNITBASIC_HPP_

#include <Hord/config.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Hive/Defs.hpp>
#include <Hord/Hive/Unit.hpp>

namespace Hord {
namespace Hive {

// Forward declarations
class UnitBasic;

/**
	@addtogroup object
	@{
*/
/**
	@addtogroup hive
	@{
*/

/**
	Basic hive unit class.
*/
class UnitBasic final
	: public Hive::Unit
{
private:
	using base = Hive::Unit;

	static Object::UPtr
	construct(
		Object::ID const id,
		Object::ID const /*parent*/
	) noexcept;

public:
	/**
		Type info.
	*/
	static constexpr Object::type_info const
	info{
		"Hord.UnitBasic",
		Hive::Type{Hive::UnitType::basic},
		{Hive::SUPPLIED_PROPS},
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
	explicit
	UnitBasic(
		Hive::ID const id
	) noexcept;
/// @}
};

/** @} */ // end of doc-group hive
/** @} */ // end of doc-group object

} // namespace Hive

template struct Hive::Unit::ensure_traits<Hive::UnitBasic>;

} // namespace Hord

#endif // HORD_HIVE_UNITBASIC_HPP_