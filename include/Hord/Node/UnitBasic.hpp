/**
@file
@brief Basic node unit class.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Node/Defs.hpp>
#include <Hord/Node/Unit.hpp>

namespace Hord {
namespace Node {

// Forward declarations
class UnitBasic;

/**
	@addtogroup object
	@{
*/
/**
	@addtogroup node
	@{
*/

/**
	Basic node unit class.
*/
class UnitBasic final
	: public Node::Unit
{
private:
	using base = Node::Unit;

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
		"Hord.Node.Basic",
		Node::Type{Node::UnitType::basic},
		{Node::SUPPLIED_PROPS},
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
		Node::ID const id,
		Object::ID const parent
	) noexcept;
/// @}
};

/** @} */ // end of doc-group node
/** @} */ // end of doc-group object

} // namespace Node

template struct Node::Unit::ensure_traits<Node::UnitBasic>;

} // namespace Hord
