/**
@file
@brief Base anchor unit class.
@ingroup anchor

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/aux.hpp>
#include <Hord/IO/PropStream.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Object/Unit.hpp>
#include <Hord/Anchor/Defs.hpp>

namespace Hord {
namespace Anchor {

// Forward declarations
class Unit;

/**
	@addtogroup anchor
	@{
*/

/**
	Base anchor unit class.

	@note This object supplies no data props.
*/
class Unit
	: public Object::Unit
{
private:
	using base = Object::Unit;

public:
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

	// Object::Unit implementation

	void
	deserialize_impl(
		IO::InputPropStream& prop_stream
	) override;

	void
	serialize_impl(
		IO::OutputPropStream& prop_stream
	) const override;

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	virtual
	~Unit() noexcept override = 0;

	/** Move constructor. */
	Unit(Unit&&);
	/** Move assignment operator. */
	Unit& operator=(Unit&&);

protected:
	/**
		Constructor with type information, ID, and parent.

		@post See Object::Unit.
	*/
	Unit(
		Object::type_info const& tinfo,
		Anchor::ID const id,
		Object::ID const parent
	) noexcept;
/// @}
};

/** @} */ // end of doc-group anchor

} // namespace Anchor
} // namespace Hord
