
#include <Hord/serialization.hpp>
#include <Hord/Log.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Object/Ops.hpp>
#include <Hord/Node/Defs.hpp>
#include <Hord/Hive/Unit.hpp>
#include <Hord/Node/Unit.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/type_info.hpp>
#include <Hord/Cmd/Unit.hpp>
#include <Hord/Cmd/Stage.hpp>
#include <Hord/Cmd/Ops.hpp>
#include <Hord/Cmd/Node.hpp>
#include <Hord/System/Context.hpp>

#include <cassert>
#include <utility>
#include <new>
#include <exception>

#include <Hord/detail/gr_core.hpp>

namespace Hord {
namespace Cmd {

#define HORD_CMD_TYPE_ NodeCreate
#define HORD_SCOPE_CLASS Cmd::Node::Create // pseudo

using ResultCode = Cmd::Node::Create::ResultCode;
using ResultData = Cmd::Node::Create::ResultData;
using Props = Cmd::Node::Create::Props;

// Declarations

namespace Node {

template<class Ser>
ser_result_type
serialize(
	ser_tag_serialize,
	Ser& ser,
	ResultData& value
) {
	auto& self = const_safe<Ser>(value);
	ser(
		Cacophony::make_enum_cfg<std::uint8_t>(self.code),
		self.id
	);
}

template<class Ser>
ser_result_type
serialize(
	ser_tag_serialize,
	Ser& ser,
	Props& value
) {
	auto& self = const_safe<Ser>(value);
	ser(
		self.parent,
		self.layout_ref,
		Cacophony::make_string_cfg<std::uint8_t>(self.slug)
	);
}

} // namespace Node

// Stage: Statement

#define HORD_CMD_STAGE_TYPE_ Statement
HORD_CMD_STAGE_DATA_OPEN()
	Hord::Node::ID id;
	Props props;

	HORD_CMD_STAGE_DATA_SERIALIZE_OPEN()
		ser(
			self.id,
			self.props
		);
	HORD_CMD_STAGE_DATA_SERIALIZE_CLOSE()
HORD_CMD_STAGE_DATA_CLOSE()
#undef HORD_CMD_STAGE_TYPE_ // Statement

// Stage: Error

#define HORD_CMD_STAGE_TYPE_ Error
HORD_CMD_STAGE_DATA_OPEN()
	ResultCode code;

	HORD_CMD_STAGE_DATA_SERIALIZE_OPEN()
		ser(
			Cacophony::make_enum_cfg<std::uint8_t>(self.code)
		);
	HORD_CMD_STAGE_DATA_SERIALIZE_CLOSE()
HORD_CMD_STAGE_DATA_CLOSE()
#undef HORD_CMD_STAGE_TYPE_ // Error

// Stage: Request

#define HORD_CMD_STAGE_TYPE_ Request
HORD_CMD_STAGE_DATA_OPEN()
	Props props;

	HORD_CMD_STAGE_DATA_SERIALIZE_OPEN()
		ser(
			self.props
		);
	HORD_CMD_STAGE_DATA_SERIALIZE_CLOSE()
HORD_CMD_STAGE_DATA_CLOSE()
#undef HORD_CMD_STAGE_TYPE_ // Request

// Stage: Response

#define HORD_CMD_STAGE_TYPE_ Response
HORD_CMD_STAGE_DATA_OPEN()
	Hord::Node::ID id;

	HORD_CMD_STAGE_DATA_SERIALIZE_OPEN()
		ser(
			self.id
		);
	HORD_CMD_STAGE_DATA_SERIALIZE_CLOSE()
HORD_CMD_STAGE_DATA_CLOSE()
#undef HORD_CMD_STAGE_TYPE_ // Response

// Type info

HORD_CMD_MAKE_STAGE_OPEN()
	HORD_CMD_MAKE_STAGE_CASE(Statement)
	HORD_CMD_MAKE_STAGE_CASE(Error)
	HORD_CMD_MAKE_STAGE_CASE(Request)
	HORD_CMD_MAKE_STAGE_CASE(Response)
HORD_CMD_MAKE_STAGE_CLOSE()

HORD_CMD_DEF_TYPE_INFO(
	HORD_SCOPE_CLASS,
	Cmd::Flags::remote,
	Cmd::Flags::revertible
);

Cmd::StageUPtr
HORD_SCOPE_CLASS::make_request(
	Props&& props
) {
	return Cmd::StageUPtr{new Request::impl({std::move(props)})};
}

// Implementation

#define HORD_SCOPE_FUNC can_create
static ResultCode
can_create(
	Hive::Unit const& hive,
	Props const& props
) noexcept {
	if (
		Object::NULL_ID != props.parent &&
		!hive.has_child(props.parent)
	) {
		return ResultCode::parent_not_found;
	} else if (
		Hord::Node::NULL_ID != props.layout_ref &&
		!hive.has_child(props.layout_ref)
	) {
		return ResultCode::layout_ref_not_found;
	} else if (
		Hord::Node::NULL_ID != props.layout_ref &&
		Object::Type::Node != hive.get_objects().find(
			props.layout_ref
		)->second->get_type()
	) {
		return ResultCode::layout_ref_not_a_node;
	} else if (props.slug.empty()) {
		return ResultCode::slug_empty;
	} else if (0xFF < props.slug.size()) {
		return ResultCode::slug_too_long;
	} else {
		for (auto const& object : hive.get_objects()) {
			if (
				props.parent == object.second->get_parent() &&
				props.slug   == object.second->get_slug()
			) {
				return ResultCode::slug_already_exists;
			}
		}
	}
	return ResultCode::ok;
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC prepare
static ResultData
prepare(
	System::Context& context,
	Props const& props,
	Cmd::Stage& /*stage*/,
	bool const generate
) {
	ResultData data{
		can_create(context.get_hive(), props),
		Hord::Node::NULL_ID
	};
	if (generate && ResultCode::ok == data.code) {
		// NB: Leak ErrorCode::datastore_closed
		data.id = context.get_datastore().generate_id(
			context.get_driver().get_id_generator()
		);
	}
	return data;
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC action
static Cmd::Status
action(
	System::Context& context,
	Props const& props,
	ResultData& data
) try {
	context.get_datastore().create_object(
		data.id,
		Object::Type::Node
	);
	auto& hive = context.get_hive();
	auto emplace_pair = hive.get_objects().emplace(
		data.id,
		Object::UPtr{new Hord::Node::Unit({
			data.id,
			Object::NULL_ID
		})}
	);
	if (!emplace_pair.second) {
		data.code = ResultCode::id_already_exists;
		return Cmd::Status::error;
	}
	hive.get_idset().emplace(data.id);
	auto& node = static_cast<Hord::Node::Unit&>(*emplace_pair.first->second);
	node.set_layout_ref(props.layout_ref);
	node.set_slug(props.slug);
	auto const it = hive.get_objects().find(props.parent);
	if (hive.get_objects().end() == it) {
		Object::set_parent(node, hive);
	} else {
		Object::set_parent(node, *it->second);
	}
	node.get_prop_states().assign_all(IO::PropState::modified);
	data.code = ResultCode::ok;
	return Cmd::Status::complete;
} catch (Hord::Error const& err) {
	Log::acquire(Log::error)
		<< HORD_SCOPE_FQN_STR_LIT
		": error creating object: "
		<< err.get_message()
	;
	if (ErrorCode::datastore_object_already_exists == err.get_code()) {
		data.code = ResultCode::id_already_exists;
	} else {
		data.code = ResultCode::unknown;
	}
	return Cmd::Status::error;
} catch (std::exception const& err) {
	Log::acquire(Log::error)
		<< HORD_SCOPE_FQN_STR_LIT
		": unknown error creating object: "
		<< err.what()
	;
	data.code = ResultCode::unknown;
	return Cmd::Status::error;
} catch (...) {
	Log::acquire(Log::error)
		<< HORD_SCOPE_FQN_STR_LIT
		": unknown error creating object"
	;
	data.code = ResultCode::unknown;
	return Cmd::Status::error;
}
#undef HORD_SCOPE_FUNC

// Stage: Statement

#define HORD_SCOPE_FUNC Statement::execute_impl
HORD_CMD_STAGE_DEF_EXECUTE(Statement) {
	(void)context; (void)command;

	// NB: Client cannot generate this stage
	assert(is_initiator());
	if (context.is_host() || is_client()) {
		return context.localized_fatal(*this);
	}

	command.result_data.code = can_create(context.get_hive(), m_data.props);
	command.result_data.id = m_data.id;
	return
		ResultCode::ok == command.result_data.code
		? action(context, m_data.props, command.result_data)
		: Cmd::Status::error
	;
}
#undef HORD_SCOPE_FUNC

// Stage: Error

#define HORD_SCOPE_FUNC Error::execute_impl
HORD_CMD_STAGE_DEF_EXECUTE(Error) {
	(void)context; (void)command;

	if (context.is_host()) {
		return context.localized_fatal(*this);
	}

	command.result_data = ResultData{
		m_data.code,
		Hord::Node::NULL_ID
	};
	return Cmd::Status::error_remote;
}
#undef HORD_SCOPE_FUNC

// Stage: Request

#define HORD_SCOPE_FUNC Request::execute_impl
HORD_CMD_STAGE_DEF_EXECUTE(Request) {
	(void)context; (void)command;

	assert(is_initiator());
	auto data = prepare(
		context,
		m_data.props,
		*this,
		context.is_host()
	);
	if (ResultCode::ok != data.code) {
		if (context.is_local(*this)) {
			command.result_data = data;
		} else {
			context.push_remote(
				*this, false, Cmd::StageUPtr{new Error::impl({data.code})}
			);
		}
		return Cmd::Status::error;
	} else if (context.is_client()) {
		context.push_remote(*this, true, Cmd::make_stage_shadow(*this));
		return Cmd::Status::waiting;
	} else {
		auto const status = action(context, m_data.props, data);
		switch (status) {
		case Cmd::Status::complete:
			context.push_remote(
				*this, false, Cmd::StageUPtr{new Response::impl({data.id})}
			);
			context.broadcast(
				Cmd::StageUPtr{new Statement::impl({data.id, m_data.props})}
			);
			break;

		case Cmd::Status::error: // fall-through
		case Cmd::Status::error_remote:
			context.push_remote(
				*this, false, Cmd::StageUPtr{new Error::impl({data.code})}
			);
			break;

		default:
			break;
		}
		return status;
	}
}
#undef HORD_SCOPE_FUNC

// Stage: Response

#define HORD_SCOPE_FUNC Response::execute_impl
HORD_CMD_STAGE_DEF_EXECUTE(Response) {
	(void)context; (void)command;

	if (context.is_host() || is_client()) {
		return context.localized_fatal(*this);
	}

	Props const& props = static_cast<Request const*>(
		command.get_initiator()->get_data()
	)->props;
	command.result_data.code = can_create(context.get_hive(), props);
	command.result_data.id = m_data.id;
	return
		ResultCode::ok == command.result_data.code
		? action(context, props, command.result_data)
		: Cmd::Status::error
	;
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS
#undef HORD_CMD_TYPE_ // NodeCreate

} // namespace Cmd
} // namespace Hord
