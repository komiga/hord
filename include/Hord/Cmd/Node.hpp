/**
@file Cmd/Node.hpp
@brief %Node commands.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/Node/Defs.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/Unit.hpp>

namespace Hord {
namespace Cmd {
namespace Node {

// Forward declarations
class Create;

/**
	@addtogroup cmd
	@{
*/
/**
	@addtogroup cmd_node
	@{
*/

/**
	Node create command.
*/
class Create final
	: public Cmd::Unit<Create>
{
	HORD_CMD_IMPL_BOILERPLATE_WITH_COMMIT(
		Create,
		"Cmd::Node::Create"
	)

private:
	Hord::Node::ID m_id{Hord::Node::ID_NULL};

public:
/** @name Properties */ /// @{
	/**
		Get ID of created node.

		@returns Node::ID_NULL if the command failed.
	*/
	Hord::Node::ID
	get_id() const noexcept {
		return m_id;
	}
/// @}

/** @name Operations */ /// @{
	/**
		Execute command.

		@note IO::PropType::identity is implicit, and any types not
		supplied for a particular object are ignored when loading
		props.
	*/
	result_type
	operator()(
		Object::ID const parent,
		Hord::Node::ID const layout_ref,
		String const& slug,
		Hord::Node::UnitType const unit_type
	) noexcept;
/// @}
};

/** @} */ // end of doc-group cmd_node
/** @} */ // end of doc-group cmd

} // namespace Node

HORD_CMD_IMPL_ENSURE_TRAITS(Cmd::Node::Create);

} // namespace Cmd
} // namespace Hord
