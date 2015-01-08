/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Base rule class.
@ingroup rule
*/

#pragma once

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

	/**
		Ensure traits for deriving classes.

		@tparam D Deriving class.
	*/
	template<class D>
	struct ensure_traits
		: base::ensure_traits<D>
	{};

private:
	Unit() = delete;
	Unit(Unit const&) = delete;
	Unit& operator=(Unit const&) = delete;

protected:
/** @name Implementation */ /// @{
	/**
		Object::Unit::deserialize_impl() implementation.
	*/
	virtual void
	deserialize_impl(
		IO::InputPropStream& prop_stream
	) override = 0;

	/**
		Object::Unit::serialize_impl() implementation.
	*/
	virtual void
	serialize_impl(
		IO::OutputPropStream& prop_stream
	) const override = 0;
/// @}

/** @name Special member functions */ /// @{
public:
	/** Destructor. */
	virtual
	~Unit() noexcept override = 0;

	/** Move constructor. */
	Unit(Unit&&);
	/** Move assignment operator. */
	Unit& operator=(Unit&&);

protected:
	/**
		Constructor with type info, %ID, and parent.

		@post See Object::Unit.
	*/
	Unit(
		Object::type_info const& tinfo,
		Rule::ID const id,
		Object::ID const parent
	) noexcept;
/// @}

public:
/** @name Properties */ /// @{
/// @}
};

/** @} */ // end of doc-group rule

} // namespace Rule
} // namespace Hord
