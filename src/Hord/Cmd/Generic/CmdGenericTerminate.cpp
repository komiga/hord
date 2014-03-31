
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/Stage.hpp>
#include <Hord/Cmd/Generic.hpp>
#include <Hord/System/Context.hpp>

#include <murk/Desc.hpp>
#include <murk/DescCompound.hpp>
#include <murk/TieCompound.hpp>
#include <murk/serialize.hpp>

#include <new>

#include <Hord/detail/gr_core.hpp>
namespace Hord {
namespace Cmd {

#define HORD_CMD_TYPE__ GenericTerminate
#define HORD_SCOPE_CLASS Cmd::Generic::Terminate // pseudo

// Stage: Statement

namespace {
static murk::DescCompound const
s_comp_statement{
	{murk::RefDesc{Cmd::Stage::s_comp_base}},
	{murk::DescType::terminate}
};

static Cmd::Stage::type_info const
s_type_info_GenericTerminate_Statement{
	Cmd::StageType::Statement,
	{s_comp_statement}
};
} // anonymous namespace

HORD_CMD_STAGE_DECL(
	Statement,
);

HORD_CMD_STAGE_DEF_BIND(
	Statement,
);

#define HORD_SCOPE_FUNC Statement::execute_impl
HORD_CMD_STAGE_DEF_EXECUTE(Statement) {
	(void)context; (void)initiator;
	return Cmd::Status::complete;
}
#undef HORD_SCOPE_FUNC

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

// Group

namespace Generic {
namespace Terminate {

Cmd::StageUPtr
make_statement() {
	return Cmd::StageUPtr{new Statement::impl()};
}

} // namespace Terminate
} // namespace Generic

#undef HORD_SCOPE_CLASS
#undef HORD_CMD_TYPE__ // GenericTerminate

} // namespace Cmd
} // namespace Hord
