/**
@file Rule/Unit.hpp
@brief Base rule class.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_RULE_UNIT_HPP_
#define HORD_RULE_UNIT_HPP_

#include <Hord/config.hpp>
#include <Hord/IO/PropStream.hpp>
#include <Hord/Object/Unit.hpp>
#include <Hord/Rule/Defs.hpp>

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

	/**
		See Object::Unit::deserialize_impl().
	*/
	virtual void
	deserialize_impl(
		IO::InputPropStream& prop_stream
	) = 0;

	/**
		See Object::Unit::serialize_impl().
	*/
	virtual void
	serialize_impl(
		IO::OutputPropStream& prop_stream
	) const = 0;
/// @}

protected:
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with ID and parent.

		@post See Object::Unit.

		@param id ID.
		@param parent Parent ID.
	*/
	Unit(
		Rule::ID const id,
		Object::ID const parent
	) noexcept;

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
