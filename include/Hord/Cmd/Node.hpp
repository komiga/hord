/**
@file Cmd/Node.hpp
@brief %Node commands.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_CMD_NODE_HPP_
#define HORD_CMD_NODE_HPP_

#include <Hord/config.hpp>
#include <Hord/String.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Node/Defs.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/type_info.hpp>
#include <Hord/Cmd/Unit.hpp>

namespace Hord {
namespace Cmd {
namespace Node {

/**
	@addtogroup cmd
	@{
*/
/**
	@addtogroup cmd_node
	@{
*/

/*

NodeCreate (Request)

* C I-> Request = [(Response & B[Statement]) | Error]
	C: W <- .
	H: * -> . & ~

* H *-> Response
	C: * & ~

* H *-> Error
	C: * & ~error_remote
	H: * & ~error

NodeCreate (Statement)

* H I-> Statement
	C: * & ~
	H: * & ~fatal

*/

/** @cond INTERNAL */
#define HORD_CMD_TYPE_ NodeCreate
/** @endcond */
HORD_CMD_DEF_OPEN(Create)
public:
	/**
		Result codes.
	*/
	enum class ResultCode {
		ok,
		parent_not_found,
		layout_ref_not_found,
		layout_ref_not_a_node,
		slug_empty,
		slug_too_long,
		slug_already_exists,
		id_already_exists,
		unknown,
	};

	/**
		Result data type.
	*/
	struct ResultData {
		ResultCode code{ResultCode::unknown};
		Hord::Node::ID id{Object::NULL_ID};

		~ResultData() = default;
		ResultData() = default;
		ResultData(ResultData const&) = default;
		ResultData& operator=(ResultData const&) = default;
		ResultData(ResultData&&) = default;
		ResultData& operator=(ResultData&&) = default;

		ResultData(
			ResultCode code,
			Hord::Node::ID id
		)
			: code(code)
			, id(id)
		{}
	};

	/**
		Command properties.
	*/
	struct Props {
		Object::ID parent;
		Hord::Node::ID layout_ref;
		String slug;
	};

/** @name Properties */ /// @{
	/** Result data. */
	ResultData result_data{};
/// @}

/** @name Operations */ /// @{
	/**
		Make @c Request stage (initiator).

		@throws std::bad_alloc
		If allocation fails.
	*/
	static Cmd::StageUPtr
	make_request(
		Props&& props
	);
/// @}
HORD_CMD_DEF_CLOSE()
#undef HORD_CMD_TYPE_

/** @} */ // end of doc-group cmd_node
/** @} */ // end of doc-group cmd

} // namespace Node
} // namespace Cmd
} // namespace Hord

#endif // HORD_CMD_NODE_HPP_
