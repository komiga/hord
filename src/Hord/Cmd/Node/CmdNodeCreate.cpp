
#include <Hord/serialization.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Node/Defs.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/Stage.hpp>
#include <Hord/Cmd/Node.hpp>
#include <Hord/System/Context.hpp>

#include <cassert>
#include <utility>
#include <new>

#include <Hord/detail/gr_core.hpp>

namespace Hord {
namespace Cmd {

#define HORD_CMD_TYPE_ NodeCreate
#define HORD_SCOPE_CLASS Cmd::Node::Create // pseudo

// Declarations

// Stage: Statement

#define HORD_CMD_STAGE_TYPE_ Statement

HORD_CMD_STAGE_DATA_OPEN()
	Hord::Node::ID id;
	Object::ID parent;
	Hord::Node::ID layout_ref;
	String slug;

	HORD_CMD_STAGE_DATA_SERIALIZE_OPEN()
		ser(
			self.id,
			self.parent,
			self.layout_ref,
			self.slug
		);
	HORD_CMD_STAGE_DATA_SERIALIZE_CLOSE()
HORD_CMD_STAGE_DATA_CLOSE()

#undef HORD_CMD_STAGE_TYPE_ // Statement

// Stage: Error

#define HORD_CMD_STAGE_TYPE_ Error

HORD_CMD_STAGE_DATA_OPEN()
	HORD_CMD_STAGE_DATA_NO_SERIALIZATION()
HORD_CMD_STAGE_DATA_CLOSE()

#undef HORD_CMD_STAGE_TYPE_ // Error

// Stage: Request

#define HORD_CMD_STAGE_TYPE_ Request

HORD_CMD_STAGE_DATA_OPEN()
	Object::ID parent;
	Hord::Node::ID layout_ref;
	String slug;

	HORD_CMD_STAGE_DATA_SERIALIZE_OPEN()
		ser(
			self.parent,
			self.layout_ref,
			self.slug
		);
	HORD_CMD_STAGE_DATA_SERIALIZE_CLOSE()
HORD_CMD_STAGE_DATA_CLOSE()

#undef HORD_CMD_STAGE_TYPE_ // Request

// Stage: Response

#define HORD_CMD_STAGE_TYPE_ Response

HORD_CMD_STAGE_DATA_OPEN()
	Hord::Node::ID id;

	HORD_CMD_STAGE_DATA_SERIALIZE_OPEN()
		ser(self.id);
	HORD_CMD_STAGE_DATA_SERIALIZE_CLOSE()
HORD_CMD_STAGE_DATA_CLOSE()

#undef HORD_CMD_STAGE_TYPE_ // Response

// Type info

HORD_CMD_CONSTRUCT_OPEN()
	HORD_CMD_CONSTRUCT_CASE(Statement)
	HORD_CMD_CONSTRUCT_CASE(Error)
	HORD_CMD_CONSTRUCT_CASE(Request)
	HORD_CMD_CONSTRUCT_CASE(Response)
HORD_CMD_CONSTRUCT_CLOSE()

HORD_CMD_DEF_TYPE_INFO(
	Cmd::Flags::remote,
	Cmd::Flags::revertible
);

// Implementation

/*

* C -> Request
	C: W <- [Response | Error] & ~
	H: * -> [Response | Error] & ~

* C -> Statement (OOB)
	H: * -> Error & ~

* H -> Statement
	C: * & ~

*/

enum class CanCreate : unsigned {
	ok,
	id_already_exists,
	parent_not_found,
	slug_already_exists,
	layout_ref_not_found,
};

/*CanCreate
can_create(
	Hive::Unit const& hive,
	Object::ID const parent,
	String const& slug,
	Hord::Node::ID const layout_ref
) noexcept {
	// TODO
	if (Hord::Node::NULL_ID != id) {
	}
	return true;
}*/

// Stage: Statement

#define HORD_SCOPE_FUNC Statement::execute_impl
HORD_CMD_STAGE_DEF_EXECUTE(Statement) {
	(void)context; (void)initiator;

	// TODO
	return Cmd::Status::complete;
}
#undef HORD_SCOPE_FUNC

// Stage: Error

#define HORD_SCOPE_FUNC Error::execute_impl
HORD_CMD_STAGE_DEF_EXECUTE(Error) {
	(void)context; (void)initiator;

	assert(context.is_client());
	return Cmd::Status::error_remote;
}
#undef HORD_SCOPE_FUNC

// Stage: Request

#define HORD_SCOPE_FUNC Request::execute_impl
HORD_CMD_STAGE_DEF_EXECUTE(Request) {
	(void)context; (void)initiator;

	assert(this == &initiator);
	if (context.is_client()) {
		// TODO: Check for errors locally first
		auto shadow = Cmd::make_stage_shadow(*this);
		context.push_output(
			*this,
			shadow,
			true
		);
		return Cmd::Status::waiting;
	} else {
		//auto& hive_pair = context.get_hive_pair();

		return Cmd::Status::error;
	}
}
#undef HORD_SCOPE_FUNC

// Stage: Response

#define HORD_SCOPE_FUNC Response::execute_impl
HORD_CMD_STAGE_DEF_EXECUTE(Response) {
	(void)context; (void)initiator;
	// TODO
	return Cmd::Status::error;
}
#undef HORD_SCOPE_FUNC

// Public interface

namespace Node {
namespace Create {

Cmd::StageUPtr
make_request(
	Object::ID const parent,
	String slug,
	Hord::Node::ID const layout_ref
) {
	return Cmd::StageUPtr{new Request::impl({
		parent,
		layout_ref,
		std::move(slug)
	})};
}

} // namespace Create
} // namespace Node

#undef HORD_SCOPE_CLASS
#undef HORD_CMD_TYPE_ // NodeCreate

} // namespace Cmd
} // namespace Hord
