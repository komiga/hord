/**
@file Cmd/Generic.hpp
@brief %Generic commands.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_CMD_GENERIC_HPP_
#define HORD_CMD_GENERIC_HPP_

#include <Hord/config.hpp>
#include <Hord/String.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/type_info.hpp>
#include <Hord/Cmd/Unit.hpp>

namespace Hord {
namespace Cmd {
namespace Generic {

// Forward declarations

/**
	@addtogroup cmd
	@{
*/
/**
	@addtogroup cmd_generic
	@{
*/

/*

NB: Injected into an existing command sequence of a completely
different command type.

GenericTerminate (Statement)

* E *-> Statement
	E: ~complete

*/

/** @cond INTERNAL */
#define HORD_CMD_TYPE_ GenericTerminate
/** @endcond */
HORD_CMD_DEF_OPEN(Terminate)
public:
	/**
		Make @c Statement stage.

		@throws std::bad_alloc
		If allocation fails.
	*/
	static Cmd::StageUPtr
	make_statement();
HORD_CMD_DEF_CLOSE()
#undef HORD_CMD_TYPE_

/** @} */ // end of doc-group cmd_generic
/** @} */ // end of doc-group cmd

} // namespace Generic
} // namespace Cmd
} // namespace Hord

#endif // HORD_CMD_GENERIC_HPP_
