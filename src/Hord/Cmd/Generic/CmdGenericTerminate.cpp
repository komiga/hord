
#include <Hord/serialization.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/Stage.hpp>
#include <Hord/Cmd/Generic.hpp>
#include <Hord/System/Context.hpp>

#include <new>

#include <Hord/detail/gr_core.hpp>
namespace Hord {
namespace Cmd {

#define HORD_CMD_TYPE_ GenericTerminate
#define HORD_SCOPE_CLASS Cmd::Generic::Terminate // pseudo

// Stage: Statement

#define HORD_CMD_STAGE_TYPE_ Statement

HORD_CMD_STAGE_DEF_TYPE_INFO();
HORD_CMD_STAGE_DATA_OPEN()
	HORD_CMD_STAGE_DATA_EMPTY();
HORD_CMD_STAGE_DATA_CLOSE()

#undef HORD_CMD_STAGE_TYPE_ // Statement

// Type info

#define HORD_SCOPE_FUNC construct_stage
static Cmd::Stage*
construct_stage(
	Cmd::StageType const type
) {
	switch (type) {
	case Cmd::StageType::Statement:
		return new Statement::impl();
	default:
		HORD_THROW_FQN(
			ErrorCode::cmd_construct_stage_type_invalid,
			"stage type not implemented"
		);
	}
}
#undef HORD_SCOPE_FUNC

Cmd::type_info const
s_type_info_GenericTerminate{
	Cmd::Type::GenericTerminate,
	{
		Cmd::Flags::none
	},
	{construct_stage}
};

// Implementation

/*

* H -> Statement
	C: * & ~

*/

// Stage: Statement

#define HORD_SCOPE_FUNC Statement::execute_impl
HORD_CMD_STAGE_DEF_EXECUTE(Statement) {
	(void)context; (void)initiator;
	return Cmd::Status::complete;
}
#undef HORD_SCOPE_FUNC

// Public interface

namespace Generic {
namespace Terminate {

Cmd::StageUPtr
make_statement() {
	return Cmd::StageUPtr{new Statement::impl()};
}

} // namespace Terminate
} // namespace Generic

#undef HORD_SCOPE_CLASS
#undef HORD_CMD_TYPE_ // GenericTerminate

} // namespace Cmd
} // namespace Hord
