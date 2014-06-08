
#include <Hord/serialization.hpp>
#include <Hord/Log.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Object/Unit.hpp>
#include <Hord/Object/Ops.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/StorageInfo.hpp>
#include <Hord/IO/PropTypeIterator.hpp>
#include <Hord/IO/PropStateStore.hpp>
#include <Hord/IO/Ops.hpp>
#include <Hord/Hive/Unit.hpp>
#include <Hord/Hive/Defs.hpp>
#include <Hord/Rule/Unit.hpp>
#include <Hord/Rule/Defs.hpp>
#include <Hord/Node/Unit.hpp>
#include <Hord/Node/Defs.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/type_info.hpp>
#include <Hord/Cmd/Unit.hpp>
#include <Hord/Cmd/Stage.hpp>
#include <Hord/Cmd/Ops.hpp>
#include <Hord/Cmd/Hive.hpp>
#include <Hord/System/Context.hpp>

#include <cassert>
#include <utility>
#include <new>
#include <exception>

#include <Hord/detail/gr_core.hpp>

namespace Hord {
namespace Cmd {

#define HORD_CMD_TYPE_ HiveInit
#define HORD_SCOPE_CLASS Cmd::Hive::Init // pseudo

using Seq = Cmd::Hive::Init::Seq;
using Props = Cmd::Hive::Init::Props;
using ResultCode = Cmd::Hive::Init::ResultCode;
using ResultData = Cmd::Hive::Init::ResultData;

// Stage: Statement

#define HORD_CMD_STAGE_TYPE_ Statement
HORD_CMD_STAGE_DATA_OPEN()
	Props props;

	HORD_CMD_STAGE_DATA_NO_SERIALIZATION()
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

struct RequestData {
	Object::ID id;
	IO::PropTypeBit prop_types;
};

#define HORD_CMD_STAGE_TYPE_ Request
HORD_CMD_STAGE_DATA_OPEN()
	RequestData data;

	HORD_CMD_STAGE_DATA_SERIALIZE_OPEN()
		ser(
			self.data.id,
			self.data.prop_types
		);
	HORD_CMD_STAGE_DATA_SERIALIZE_CLOSE()
HORD_CMD_STAGE_DATA_CLOSE()
#undef HORD_CMD_STAGE_TYPE_ // Request

// Stage: Response

struct ResponseData {
	enum : unsigned {
		GROWTH_RATE = 2048u
	};

	DynamicStreamBuf buffer{0u, GROWTH_RATE};

	ResponseData() = default;
	ResponseData(ResponseData&&) = default;
	ResponseData& operator=(ResponseData&&) = default;

	ResponseData(
		ResponseData const& /*other*/
	) noexcept
		: buffer(0u, GROWTH_RATE)
	{}

	ResponseData(
		DynamicStreamBuf&& buffer
	) noexcept
		: buffer(std::move(buffer))
	{}
};

#define HORD_CMD_STAGE_TYPE_ Response
HORD_CMD_STAGE_DATA_OPEN()
	ResponseData data;

	HORD_CMD_STAGE_DATA_SERIALIZE_OPEN()
		ser(
			self.data.buffer
		);
	HORD_CMD_STAGE_DATA_SERIALIZE_CLOSE()
HORD_CMD_STAGE_DATA_CLOSE()
#undef HORD_CMD_STAGE_TYPE_ // Response

// Stage: Signal

#define HORD_CMD_STAGE_TYPE_ Signal
HORD_CMD_STAGE_DATA_OPEN()
	HORD_CMD_STAGE_DATA_NO_SERIALIZATION()
HORD_CMD_STAGE_DATA_CLOSE()
#undef HORD_CMD_STAGE_TYPE_ // Signal

// Type info

HORD_CMD_MAKE_STAGE_OPEN()
	HORD_CMD_MAKE_STAGE_CASE(Statement)
	HORD_CMD_MAKE_STAGE_CASE(Error)
	HORD_CMD_MAKE_STAGE_CASE(Request)
	HORD_CMD_MAKE_STAGE_CASE(Response)
	HORD_CMD_MAKE_STAGE_CASE(Signal)
HORD_CMD_MAKE_STAGE_CLOSE()

HORD_CMD_DEF_TYPE_INFO(
	HORD_SCOPE_CLASS,
	Cmd::Flags::remote
);

Cmd::StageUPtr
HORD_SCOPE_CLASS::make_statement(
	Props&& props
) {
	return Cmd::StageUPtr{new Statement::impl({std::move(props)})};
}

// Implementation

#define HORD_SCOPE_FUNC link_parents
void
link_parents(
	Hord::Hive::Unit& hive
) {
	auto& objects = hive.get_objects();
	for (auto& obj_pair : objects) {
		auto& obj = *obj_pair.second;
		auto parent = obj.get_parent();
		if (obj.get_id() == parent) {
			Log::acquire(Log::error)
				<< "Object " << obj << " has itself as parent\n"
			;
			parent = Object::ID_HIVE;
		} else if (
			Object::ID_HIVE != obj.get_parent() && (
				Object::ID_NULL == obj.get_parent() ||
				!hive.has_object(obj.get_parent())
			)
		) {
			Log::acquire(Log::error)
				<< "Object " << obj << " points to invalid parent: "
				<< Object::IDPrinter{obj.get_parent()}
				<< "\n"
			;
			parent = Object::ID_HIVE;
		}
		// NB: Ignoring circular & invalid parenting; client should
		// decide what to do with these objects
		Object::set_parent(obj, hive, parent);
	}
}
#undef HORD_SCOPE_FUNC

// Stage: Statement

#define HORD_SCOPE_FUNC Statement::execute_impl
HORD_CMD_STAGE_DEF_EXECUTE(Statement) {
	(void)context; (void)command;

	if (!context.is_local(*this)) {
		return Cmd::Status::error_remote;
	}

	if (context.get_hive().get_prop_states().is_initialized(
		IO::PropType::identity
	)) {
		Log::acquire()
			<< HORD_SCOPE_FQN_STR_LIT
			": hive already initialized"
		;
		return Cmd::Status::complete;
	}

	command.prop_types = enum_combine(
		IO::PropTypeBit::identity,
		m_data.props.prop_types
	);
	if (context.is_client()) {
		// At Seq::load_storage_info; initiating request gives us
		// storage info
		context.push_remote(
			*this, true,
			Cmd::StageUPtr{new Request::impl({{
				Object::ID_NULL,
				IO::PropTypeBit::none
			}})}
		);
		return Cmd::Status::waiting;
	} else try {
		// NB: Errors from HiveInit should be treated as fatal, so
		// not worrying about resetting the hive state if an error
		// occurs
		auto const& driver = context.get_driver();
		auto& hive = context.get_hive();
		auto& datastore = context.get_datastore();
		IO::load_props(
			datastore, hive,
			hive.get_prop_states().supplied_conj(command.prop_types)
		);
		auto const& si_map = make_const(datastore).get_storage_info();
		for (auto const& si_pair : si_map) {
			auto const& sinfo = si_pair.second;
			if (IO::Linkage::resident != sinfo.linkage) {
				continue;
			}
			auto const* const
			tinfo = driver.get_object_type_info(sinfo.object_type);
			if (nullptr == tinfo) {
				command.result.code = ResultCode::object_type_unknown;
				return Cmd::Status::error;
			}
			auto const emplace_pair = hive.get_objects().emplace(
				sinfo.object_id,
				tinfo->construct(
					sinfo.object_id,
					Object::ID_NULL
				)
			);
			if (!emplace_pair.second) {
				command.result.code = ResultCode::object_already_exists;
				return Cmd::Status::error;
			} else if (nullptr == emplace_pair.first->second) {
				command.result.code = ResultCode::object_allocation_failed;
				return Cmd::Status::error;
			}
			auto& obj = *emplace_pair.first->second;
			IO::load_props(
				datastore, obj,
				obj.get_prop_states().supplied_conj(command.prop_types)
			);
		}
		link_parents(hive);
		return Cmd::Status::complete;
	} catch (Hord::Error const& err) {
		Log::acquire(Log::error)
			<< HORD_SCOPE_FQN_STR_LIT
			": error storing prop: "
			<< err.get_message()
		;
		switch (err.get_code()) {
		case ErrorCode::datastore_closed: // fall-through
		case ErrorCode::datastore_locked:
			command.result.code = ResultCode::datastore_invalid_state;
			break;
		case ErrorCode::serialization_prop_improper_state:
			command.result.code = ResultCode::prop_uninitialized;
			break;
		case ErrorCode::serialization_io_failed:
			command.result.code = ResultCode::serialization_failed;
			break;

		default:
			command.result.code = ResultCode::unknown_error;
			break;
		}
		return Cmd::Status::error;
	}
}
#undef HORD_SCOPE_FUNC

// Stage: Error

#define HORD_SCOPE_FUNC Error::execute_impl
HORD_CMD_STAGE_DEF_EXECUTE(Error) {
	(void)context; (void)command;

	if (context.is_host() || is_host()) {
		return context.localized_fatal(*this);
	}
	command.result.code = m_data.code;
	return Cmd::Status::error_remote;
}
#undef HORD_SCOPE_FUNC

// Stage: Request

#define HORD_SCOPE_FUNC request_action
static ResultCode
request_action_first(
	System::Context& context,
	ResponseData& response
) try {
	if (!response.buffer.reset()) {
		return ResultCode::unknown_error;
	}
	std::ostream stream{&response.buffer};
	auto ser = make_output_serializer(stream);
	auto const& si_map = make_const(context.get_datastore()).get_storage_info();
	ser(static_cast<std::uint32_t>(si_map.size()));
	for (auto const& si_pair : si_map) {
		ser(si_pair.second);
	}
	return ResultCode::ok;
} catch (SerializerError const& err) {
	Log::acquire(Log::error)
		<< HORD_SCOPE_FQN_STR_LIT
		": serializer error handling initiating request: "
		<< err.get_message()
	;
	return ResultCode::serialization_failed;
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC request_action
static ResultCode
request_action(
	System::Context& context,
	RequestData const& request,
	ResponseData& response
) try {
	auto& datastore = context.get_datastore();
	auto& hive = context.get_hive();
	auto& objects = hive.get_objects();
	auto it = objects.end();
	if (Object::ID_HIVE != request.id) {
		it = objects.find(request.id);
		if (objects.end() == it) {
			return ResultCode::object_not_found;
		}
	}
	auto& obj = objects.end() != it ? *it->second : hive;
	if (!obj.get_prop_states().supplies_all(request.prop_types)) {
		return ResultCode::prop_unsupplied;
	}
	if (!response.buffer.reset()) {
		return ResultCode::unknown_error;
	}
	std::ostream stream{&response.buffer};
	IO::store_props_weak(datastore, obj, request.prop_types, stream, true);
	return ResultCode::ok;
} catch (Hord::Error const& err) {
	Log::acquire(Log::error)
		<< HORD_SCOPE_FQN_STR_LIT
		": error making response: "
		<< err.get_message()
	;
	switch (err.get_code()) {
	case ErrorCode::datastore_closed: // fall-through
	case ErrorCode::datastore_locked:
		return ResultCode::datastore_invalid_state;

	case ErrorCode::serialization_prop_improper_state:
		return ResultCode::prop_uninitialized;

	case ErrorCode::serialization_data_malformed: // fall-through
	case ErrorCode::serialization_io_failed:
		return ResultCode::serialization_failed;

	default:
		return ResultCode::unknown_error;
	}
} catch (std::exception const& err) {
	Log::acquire(Log::error)
		<< HORD_SCOPE_FQN_STR_LIT
		": unknown error making response: "
		<< err.what()
	;
	return ResultCode::unknown_error;
} catch (...) {
	Log::acquire(Log::error)
		<< HORD_SCOPE_FQN_STR_LIT
		": unknown error making response"
	;
	return ResultCode::unknown_error;
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC Request::execute_impl
HORD_CMD_STAGE_DEF_EXECUTE(Request) {
	(void)context; (void)command;

	if (context.is_client() || is_host()) {
		return context.localized_fatal(*this);
	}

	ResponseData response{};
	ResultCode const result_code
		= is_initiator()
		? request_action_first(context, response)
		: request_action(context, m_data.data, response)
	;
	if (ResultCode::ok == result_code) {
		context.push_remote(
			*this, false,
			Cmd::StageUPtr{new Response::impl({std::move(response)})}
		);
		return Cmd::Status::waiting;
	} else {
		context.push_remote(
			*this, false,
			Cmd::StageUPtr{new Error::impl({result_code})}
		);
		return Cmd::Status::error;
	}
}
#undef HORD_SCOPE_FUNC

// Stage: Response

#define HORD_SCOPE_FUNC Response::execute_impl
ResultCode
response_action_first(
	System::Context& context,
	ResponseData& response
) try {
	std::istream stream{&response.buffer};
	auto ser = make_input_serializer(stream);
	auto const& driver = context.get_driver();
	auto& datastore = context.get_datastore();
	auto& objects = context.get_hive().get_objects();
	std::uint32_t size = 0u;
	ser(size);
	IO::StorageInfo sinfo{
		Object::ID_NULL,
		Object::TYPE_NULL,
		{false, false},
		IO::Linkage::resident
	};
	while (size--) {
		ser(sinfo);
		auto const* const
		tinfo = driver.get_object_type_info(sinfo.object_type);
		if (nullptr == tinfo) {
			return ResultCode::object_type_unknown;
		}
		datastore.create_object(sinfo.object_id, *tinfo, sinfo.linkage);
		if (IO::Linkage::resident != sinfo.linkage) {
			continue;
		}
		auto const emplace_pair = objects.emplace(
			sinfo.object_id,
			tinfo->construct(
				sinfo.object_id,
				Object::ID_NULL
			)
		);
		if (!emplace_pair.second) {
			return ResultCode::object_already_exists;
		} else if (nullptr == emplace_pair.first->second) {
			return ResultCode::object_allocation_failed;
		}
	}
	return ResultCode::ok;
} catch (SerializerError const& err) {
	Log::acquire(Log::error)
		<< HORD_SCOPE_FQN_STR_LIT
		": serializer error handling response: "
		<< err.get_message()
	;
	return ResultCode::serialization_failed;
} catch (Hord::Error const& err) {
	Log::acquire(Log::error)
		<< HORD_SCOPE_FQN_STR_LIT
		": error handling response: "
		<< err.get_message()
	;
	switch (err.get_code()) {
	case ErrorCode::datastore_closed: // fall-through
	case ErrorCode::datastore_locked:
		return ResultCode::datastore_invalid_state;
	case ErrorCode::datastore_object_already_exists:
		return ResultCode::object_already_exists;

	default:
		return ResultCode::unknown_error;
	}
} catch (std::exception const& err) {
	Log::acquire(Log::error)
		<< HORD_SCOPE_FQN_STR_LIT
		": unknown error handling response: "
		<< err.what()
	;
	return ResultCode::unknown_error;
} catch (...) {
	Log::acquire(Log::error)
		<< HORD_SCOPE_FQN_STR_LIT
		": unknown error handling response"
	;
	return ResultCode::unknown_error;
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC Response::execute_impl
ResultCode
response_action(
	System::Context& context,
	ResponseData& response,
	Object::Unit& obj,
	IO::PropTypeBit prop_types
) try {
	auto& datastore = context.get_datastore();
	prop_types = obj.get_prop_states().supplied_conj(prop_types);
	std::istream stream{&response.buffer};
	IO::load_props_weak(datastore, obj, prop_types, stream, true);
	return ResultCode::ok;
} catch (Hord::Error const& err) {
	Log::acquire(Log::error)
		<< HORD_SCOPE_FQN_STR_LIT
		": error handling response: "
		<< err.get_message()
	;
	switch (err.get_code()) {
	case ErrorCode::datastore_closed: // fall-through
	case ErrorCode::datastore_locked:
		return ResultCode::datastore_invalid_state;
	case ErrorCode::datastore_object_already_exists:
		return ResultCode::object_already_exists;

	default:
		return ResultCode::unknown_error;
	}
} catch (std::exception const& err) {
	Log::acquire(Log::error)
		<< HORD_SCOPE_FQN_STR_LIT
		": unknown error handling response: "
		<< err.what()
	;
	return ResultCode::unknown_error;
} catch (...) {
	Log::acquire(Log::error)
		<< HORD_SCOPE_FQN_STR_LIT
		": unknown error handling response"
	;
	return ResultCode::unknown_error;
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC Response::execute_impl
HORD_CMD_STAGE_DEF_EXECUTE(Response) {
	(void)context; (void)command;

	if (context.is_host() || is_host()) {
		return context.localized_fatal(*this);
	}

	auto& hive = context.get_hive();
	ResultCode result_code = ResultCode::unknown_error;
	switch (command.m_seq) {
	case Seq::load_storage_info:
		result_code = response_action_first(context, m_data.data);
		command.m_seq = Seq::load_hive;
		break;

	case Seq::load_hive:
		result_code = response_action(
			context, m_data.data, hive, command.prop_types
		);
		command.m_seq = Seq::load_objects;
		command.m_pos = hive.get_objects().begin();
		break;

	case Seq::load_objects:
		auto& obj = *command.m_pos->second;
		result_code = response_action(
			context, m_data.data, obj, command.prop_types
		);
		++command.m_pos;
		break;
	}

	if (ResultCode::ok == result_code) {
		if (Seq::load_hive == command.m_seq) {
			context.push_remote(
				*this, false,
				Cmd::StageUPtr{new Request::impl({{
					Object::ID_HIVE,
					hive.get_prop_states().supplied_conj(command.prop_types)
				}})}
			);
			return Cmd::Status::waiting;
		} else if (hive.get_objects().end() == command.m_pos) {
			// Seq::load_objects, reached end
			link_parents(hive);
			context.push_remote(
				*this, false,
				Cmd::StageUPtr{new Signal::impl()}
			);
			return Cmd::Status::complete;
		} else {
			// Seq::load_objects
			auto const& obj = *command.m_pos->second;
			context.push_remote(
				*this, false,
				Cmd::StageUPtr{new Request::impl({{
					obj.get_id(),
					obj.get_prop_states().supplied_conj(command.prop_types)
				}})}
			);
			return Cmd::Status::waiting;
		}
	} else {
		context.push_remote(
			*this, false,
			Cmd::StageUPtr{new Error::impl({result_code})}
		);
		command.result.code = result_code;
		return Cmd::Status::error;
	}
}
#undef HORD_SCOPE_FUNC

// Stage: Signal

#define HORD_SCOPE_FUNC Signal::execute_impl
HORD_CMD_STAGE_DEF_EXECUTE(Signal) {
	(void)context; (void)command;

	if (context.is_client() || is_host()) {
		return context.localized_fatal(*this);
	}
	command.result.code = ResultCode::ok;
	return Cmd::Status::complete;
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS
#undef HORD_CMD_TYPE_ // HiveInit

} // namespace Cmd
} // namespace Hord
