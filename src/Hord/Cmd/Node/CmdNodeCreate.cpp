
#include <Hord/Error.hpp>
#include <Hord/Hive/Defs.hpp>
#include <Hord/Node/Defs.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/Stage.hpp>
#include <Hord/Cmd/Node.hpp>
#include <Hord/System/Context.hpp>

#include <murk/Desc.hpp>
#include <murk/DescCompound.hpp>
#include <murk/TieCompound.hpp>
#include <murk/serialize.hpp>

#include <utility>
#include <new>

namespace Hord {
namespace Cmd {

#define HORD_CMD_TYPE__ NodeCreate
#define HORD_SCOPE_CLASS_IDENT__ Cmd::Node::Create // pseudo

// Stage: Request

namespace {
static murk::DescCompound const
s_comp_request{
	{murk::RefDesc{Cmd::Stage::s_comp_base}},
	{murk::DescType::uint32},
	{murk::DescType::uint32},
	{murk::StringDesc{0u}},
	{murk::DescType::terminate}
};

static Cmd::Stage::type_info const
s_type_info_NodeCreate_Request{
	Cmd::StageType::Request,
	{s_comp_request}
};
} // anonymous namespace

HORD_CMD_STAGE_DECL(
	Request,
	Hord::Hive::ID owner;
	Hord::Node::ID layout_ref;
	String slug;
);

HORD_CMD_STAGE_DEF_BIND(
	Request,
	(&m_data.owner)
	(&m_data.layout_ref)
	(&m_data.slug)
);

#define HORD_SCOPE_FUNC_IDENT__ ::Request::execute_impl
HORD_CMD_STAGE_DEF_EXECUTE(Request) {
	(void)context; (void)initiator;
	// TODO: queue Request / Response

	return Cmd::Status::waiting;
}
#undef HORD_SCOPE_FUNC_IDENT__

// Stage: Response

namespace {
static murk::DescCompound const
s_comp_response{
	{murk::RefDesc{Cmd::Stage::s_comp_base}},
	{murk::DescType::uint32},
	{murk::DescType::terminate}
};

static Cmd::Stage::type_info const
s_type_info_NodeCreate_Response{
	Cmd::StageType::Response,
	{s_comp_response}
};
} // anonymous namespace

HORD_CMD_STAGE_DECL(
	Response,
	Hord::Node::ID id;
);

HORD_CMD_STAGE_DEF_BIND(
	Response,
	(&m_data.id)
);

#define HORD_SCOPE_FUNC_IDENT__ ::Response::execute_impl
HORD_CMD_STAGE_DEF_EXECUTE(Response) {
	(void)context; (void)initiator;
	// TODO
	return Cmd::Status::error;
}
#undef HORD_SCOPE_FUNC_IDENT__

// Type info

#define HORD_SCOPE_FUNC_IDENT__ construct_stage
static Cmd::Stage*
construct_stage(
	Cmd::StageType const type
) {
	switch (type) {
	case Cmd::StageType::Request:
		return new Request::impl();
	case Cmd::StageType::Response:
		return new Response::impl();
	default:
		HORD_THROW_ERROR_SCOPED_FQN(
			ErrorCode::cmd_construct_stage_type_invalid,
			"stage type not implemented"
		);
	}
}
#undef HORD_SCOPE_FUNC_IDENT__

Cmd::type_info const
s_type_info_NodeCreate{
	Cmd::Type::NodeCreate,
	{
		Cmd::Flags::remote,
		Cmd::Flags::revertible
	},
	{construct_stage}
};

// Group

namespace Node {

Cmd::StageUPtr
make_create(
	Hord::Hive::ID const owner,
	String slug,
	Hord::Node::ID const layout_ref
) {
	return Cmd::StageUPtr{new Request::impl({
		owner,
		layout_ref,
		std::move(slug)
	})};
}

} // namespace Node

#undef HORD_SCOPE_CLASS_IDENT__
#undef HORD_CMD_TYPE__ // NodeCreate

} // namespace Cmd
} // namespace Hord
