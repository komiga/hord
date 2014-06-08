/**
@file Cmd/Hive.hpp
@brief %Hive commands.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_CMD_NODE_HPP_
#define HORD_CMD_NODE_HPP_

#include <Hord/config.hpp>
#include <Hord/serialization.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Hive/Defs.hpp>
#include <Hord/Hive/Unit.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/Prop.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/type_info.hpp>
#include <Hord/Cmd/Unit.hpp>

namespace Hord {
namespace Cmd {
namespace Hive {

/**
	@addtogroup cmd
	@{
*/
/**
	@addtogroup cmd_hive
	@{
*/

/*

TODO: HiveInit needs to prevent all other commands from running to
avoid race conditions (e.g., NodeCreate while running, or, in general,
mutating the hive elsewhere).

HiveInit (Statement + ad-hoc)

* H I-> Statement
	H: ~

--

* C I-> Statement
	C:	(-> I:Request & W) |
		~error

* C *-> [I:Request | Request]
	H:	(-> Response & W) |
		(-> Error    & ~error)

* H *-> Response
	C:	(-> Request & W) |
		(-> Error   & ~error) |
		(-> Signal  & ~)

* C *-> Signal
	H: ~

* E *-> Error
	E: ~error_remote

*/

/** @cond INTERNAL */
#define HORD_CMD_TYPE_ HiveInit
/** @endcond */
HORD_CMD_DEF_OPEN(Init)
public:
	/** @cond INTERNAL */
	enum class Seq : unsigned {
		load_storage_info,
		load_hive,
		load_objects,
	};
	/** @endcond */ // INTERNAL

private:
	Seq m_seq{Seq::load_storage_info};
	// NB: Not used on host. Client requests the exact props it wants
	// to ensure the response it gets is expected
	IO::PropTypeBit prop_types{IO::PropTypeBit::none};
	Hord::Hive::Unit::object_map_type::iterator m_pos{};

public:
	/**
		Result codes.
	*/
	enum class ResultCode : unsigned {
		ok,
		unknown_error,
		object_not_found,
		object_type_mismatch,
		object_type_unknown,
		object_already_exists,
		object_allocation_failed,
		serialization_failed,
		prop_uninitialized,
		prop_unsupplied,
		datastore_invalid_state,
	};

	/**
		Properties.
	*/
	struct Props {
		/**
			Properties to load for all objects.

			IO::PropType::identity is implicit, and any types not
			supplied for a particular object are ignored when loading
			props.
		*/
		IO::PropTypeBit prop_types;
	};

	/**
		Result data.
	*/
	struct ResultData {
		/** Result code. */
		ResultCode code;
	};

/** @name Properties */ /// @{
	/** Result data. */
	ResultData result{
		ResultCode::ok
	};
/// @}

/** @name Stages */ /// @{
	/**
		Make %Statement stage (initiator).

		@throws std::bad_alloc
		If allocation fails.
	*/
	static Cmd::StageUPtr
	make_statement(
		Props&& props
	);
/// @}
HORD_CMD_DEF_CLOSE()
#undef HORD_CMD_TYPE_

/** @} */ // end of doc-group cmd_hive
/** @} */ // end of doc-group cmd

} // namespace Hive
} // namespace Cmd
} // namespace Hord

#endif // HORD_CMD_NODE_HPP_
