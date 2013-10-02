
#include <Hord/utility.hpp>
#include <Hord/Error.hpp>
#include <Hord/System/Context.hpp>

#include <cassert>

namespace Hord {
namespace System {

// class Context implementation

#define HORD_SCOPE_CLASS_IDENT__ System::Context

Context::Context(
	Type const type,
	System::Driver& driver,
	Hive::ID const hive_id
) noexcept
	: m_type(type)
	, m_driver(driver)
	, m_hive_id(hive_id)
	, m_genid(Cmd::NULL_ID)
	, m_active()
	, m_input()
	, m_output()
{}

Context::Context(Context&&) noexcept = default;
Context::~Context() noexcept = default;

//Context& Context::operator=(Context&&) = default;

Cmd::ID
Context::next_id() noexcept {
	if (Cmd::MAX_ID == m_genid) {
		return m_genid = 1u;
	} else {
		return ++m_genid;
	}
}

void
Context::terminate(
	Cmd::ID const /*id*/
) noexcept {
	// TODO
}

void
Context::initiate(
	Cmd::StageUPtr& initiator
) noexcept {
	assert(Cmd::NULL_ID == initiator->get_id());
	m_input.emplace_back(std::move(initiator));
}

#define HORD_SCOPE_FUNC_IDENT__ push_input
void
Context::push_input(
	Cmd::StageUPtr& stage
) noexcept {
	assert(Cmd::NULL_ID != stage->get_id());
	/*bool const is_init = stage->get_id_fields().flag_remote_initiator;
	auto const it = make_const(m_active).find(stage->get_id());
	if (m_active.cend() == it) {
		if (!is_init)
			HORD_THROW_ERROR_SCOPED_FQN(
				ErrorCode::context_input_not_active,
				"non-initiating stage carries an ID which is not"
				" active"
			);
		}
	} else if (is_init) { // command is active (per first branch)
		HORD_THROW_ERROR_SCOPED_FQN(
			ErrorCode::context_input_already_active,
			"initiating stage carries an ID which is already"
			" active"
		);
	}*/
	m_input.emplace_back(std::move(stage));
}
#undef HORD_SCOPE_FUNC_IDENT__

#define HORD_SCOPE_FUNC_IDENT__ push_output
void
Context::push_output(
	Cmd::Stage const& origin,
	Cmd::StageUPtr& stage,
	bool const remote_initiator
) {
	assert(Cmd::NULL_ID != origin.get_id());
	assert(Cmd::NULL_ID == stage->get_id());

	Cmd::ID const origin_id = origin.get_id();
	auto const it = make_const(m_active).find(origin_id);
	if (m_active.cend() == it) {
		HORD_THROW_ERROR_SCOPED_FQN(
			ErrorCode::context_output_detached,
			"origin stage does not belong to an active command"
		);
	} else {
		Cmd::IDFields& fields = stage->get_id_fields();
		fields.value = origin_id;
		fields.flag_remote_initiator = remote_initiator;
		m_output.emplace_back(std::move(stage));
	}
}
#undef HORD_SCOPE_FUNC_IDENT__

#define HORD_SCOPE_FUNC_IDENT__ execute_input
std::size_t
Context::execute_input(
	id_status_pair_type& result
) {
	// 1. null id: local initiator (ensured by encapsulation).
	// 2. inactive non-null id not flagged as remote initiator:
	// error.
	// 3. active non-null id flagged as remote initiator: error.
	// 4. inactive non-null id flagged as remote initiator:
	// remote initiator.
	// 5. otherwise: execute.

	// TODO: logging
	result.first = Cmd::NULL_ID;
	result.second = static_cast<Cmd::Status>(0u);
	if (m_input.empty()) {
		return 0u;
	}

	// NB: If the stage is not an initiator (i.e., emplaced),
	// it will be destroyed when this function terminates.
	Cmd::StageUPtr stage_ptr{std::move(m_input.back())};
	Cmd::Stage& stage = *stage_ptr.get();
	m_input.pop_back();

	bool initiate = false;
	stage_map_type::iterator it;
	if (Cmd::NULL_ID == stage.get_id()) { // 1
		stage.get_id_fields().value = next_id();
		initiate = true;
	} else { // 2-4
		bool const is_init = stage.get_id_fields().flag_remote_initiator;
		it = m_active.find(stage.get_id());
		if (m_active.end() == it) {
			if (!is_init) { // 2
				HORD_THROW_ERROR_SCOPED_FQN(
					ErrorCode::context_execute_not_active,
					"non-initiating stage carries an ID which is not"
					" active"
				);
			} else { // 4
				initiate = true;
			}
		} else if (is_init) { // 3
			HORD_THROW_ERROR_SCOPED_FQN(
				ErrorCode::context_execute_already_active,
				"initiating stage carries an ID which is already"
				" active"
			);
		}
	}

	if (initiate) {
		// TODO: handle ID collision (check flag_remote_initiator)
		auto const&& emplace_result = m_active.emplace(
			stage.get_id(),
			std::move(stage_ptr)
		);
		assert(emplace_result.second); // success
		it = emplace_result.first;
	}

	// execute
	// TODO: specially handle GenericTerminate input
	result.first = stage.get_id();
	try {
		result.second = stage.execute(*this, *it->second.get());
	} catch (...) {
		// TODO: terminate command
		// TODO: push GenericTerminate output
		result.second = Cmd::Status::error; // implicit
		throw;
	}
	return m_input.size();
}
#undef HORD_SCOPE_FUNC_IDENT__

#undef HORD_SCOPE_CLASS_IDENT__

} // namespace System
} // namespace Hord
