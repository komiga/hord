/**
@file Rule/Unit.hpp
@brief Base rule class.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_RULE_UNIT_HPP_
#define HORD_RULE_UNIT_HPP_

#include <Hord/config.hpp>
#include <Hord/Rule/Defs.hpp>
#include <Hord/Object/Unit.hpp>

// TODO: Standard Rule classes

namespace Hord {
namespace Rule {

// Forward declarations
class Unit;

/**
	@addtogroup object
	@{
*/
/**
	@addtogroup rule
	@{
*/

/**
	Base rule.
*/
class Unit
	: public Object::Unit
{
public:
	/** Base class. */
	using base = Object::Unit;

	/** Type info. */
	using type_info = Rule::type_info;
	using base::ensure_traits;

private:
	Unit() = delete;
	Unit(Unit const&) = delete;
	Unit& operator=(Unit const&) = delete;

	Object::type_info const&
	get_type_info_impl() const noexcept override;

protected:
/** @name Implementation */ /// @{
	/**
		get_rule_type_info() implementation.
	*/
	virtual type_info const&
	get_rule_type_info_impl() const noexcept = 0;
/// @}

protected:
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with owner and ID.

		@post See Object::Unit().

		@param supplies_primary Whether IO::PropType::primary is
		supplied by the object.
		@param supplies_auxiliary Whether IO::PropType::auxiliary is
		supplied by the object.
		@param owner Owner.
		@param id ID.
	*/
	Unit(
		bool const supplies_primary,
		bool const supplies_auxiliary,
		Hive::ID const owner,
		Rule::ID const id
	) noexcept
		: base(
			supplies_primary,
			supplies_auxiliary,
			static_cast<Object::ID>(owner),
			static_cast<Object::ID>(id)
		)
	{}

	/** Move constructor. */
	Unit(Unit&&);
public:
	/** Destructor. */
	virtual
	~Unit() noexcept override = 0;
/// @}

protected:
/** @name Operators */ /// @{
	/** Move assignment operator. */
	Unit& operator=(Unit&&);
/// @}

public:
/** @name Properties */ /// @{
	/**
		Get type info.
	*/
	type_info const&
	get_rule_type_info() const noexcept {
		return get_rule_type_info_impl();
	}
/// @}
};

/** @} */ // end of doc-group rule
/** @} */ // end of doc-group object

} // namespace Rule
} // namespace Hord

#endif // HORD_RULE_UNIT_HPP_
