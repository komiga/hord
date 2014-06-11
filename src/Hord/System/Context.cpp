
#include <Hord/utility.hpp>
#include <Hord/Log.hpp>
#include <Hord/System/Driver.hpp>
#include <Hord/System/Context.hpp>
#include <Hord/Cmd/Ops.hpp>
#include <Hord/Cmd/Generic.hpp>

#include <cassert>

#include <Hord/detail/gr_core.hpp>

namespace Hord {
namespace System {

// class Context implementation

enum : Cmd::IDValue {
	BASE_GENID = Cmd::IDValue{1u}
};

#define HORD_SCOPE_CLASS System::Context

#define HORD_SCOPE_FUNC ctor // pseudo
System::Driver::datastore_hive_pair&
get_hive_pair(
	System::Driver& driver,
	Hive::ID const hive_id
) {
	auto const it = driver.find_hive(hive_id);
	if (driver.get_hives().end() == it) {
		HORD_THROW_FQN(
			ErrorCode::context_invalid_hive,
			"hive ID does not exist within driver"
		);
	}
	return it->second;
}
#undef HORD_SCOPE_FUNC

Context::Context(
	Type const type,
	System::Driver& driver,
	System::Driver::datastore_hive_pair& hive_pair
)
	: m_type(type)
	, m_driver(driver)
	, m_datastore(*hive_pair.datastore)
	, m_hive(static_cast<Hive::Unit&>(*hive_pair.hive))
	, m_genid(Cmd::ID_NULL.value())
	, m_active()
	, m_done()
	, m_input()
	, m_output()
{}

Context::~Context() noexcept = default;

Context::Context(Context&&) noexcept = default;

Context::Context(
	Type const type,
	System::Driver& driver,
	Hive::ID const hive_id
)
	: Context(
		type,
		driver,
		get_hive_pair(driver, hive_id)
	)
{}

// commands

Cmd::IDValue
Context::next_id() noexcept {
	if (Cmd::ID_MAX.value() == m_genid) {
		return m_genid = BASE_GENID;
	} else {
		return ++m_genid;
	}
}

#define HORD_SCOPE_FUNC terminate
void
Context::terminate(
	Cmd::Stage& input_stage,
	command_map_type::iterator it,
	bool const push_terminator
) noexcept {
	// FIXME: `it` could be invalidated from stage execution
	if (m_active.end() == it) {
		Log::acquire()
			<< "remote terminated inactive command: "
			<< Cmd::IDPrinter{input_stage}
		;
		return;
	}

	Log::acquire()
		<< "terminating command: "
		<< input_stage
	;
	auto const id = it->second->get_id_canonical();
	// Purge input
	for (
		input_deque_type::size_type idx = 0;
		m_input.size() > idx;
	) {
		if (m_input[idx]->get_id_canonical() == id) {
			Log::acquire()
				<< "discarding input stage (command termination): "
				<< *m_input[idx]
			;
			m_input.erase(m_input.cbegin() + idx);
		} else {
			++idx;
		}
	}

	// Purge output
	for (
		output_deque_type::size_type idx = 0;
		m_output.size() > idx;
	) {
		auto& output = m_output[idx];
		if (
			output.stage->get_id_canonical() == id
		) {
			Log::acquire()
				<< "discarding output stage (command termination): "
				<< *output.stage
			;
			m_output.erase(m_output.cbegin() + idx);
		} else {
			++idx;
		}
	}

	// Push terminator
	if (push_terminator && !is_local_initiator(input_stage)) {
		auto terminator = Cmd::Generic::Terminate::make_statement();
		terminator->get_id().assign(
			it->second->get_id(),
			false
		);
		m_output.emplace_back(Dest::remote, std::move(terminator));
	}

	// Finally, terminate the damned thing locally
	m_done.emplace_back(std::move(it->second));
	m_active.erase(it);
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC initiate
Cmd::IDValue
Context::initiate(
	Cmd::UnitUPtr command,
	Cmd::StageUPtr initiator
) noexcept {
	assert(!command->is_identified());
	assert(!command->has_initiator());
	assert(!initiator->is_identified());
	assert(initiator->get_command_type() == command->get_type());

	Cmd::ID const id{next_id(), is_host()};
	command->get_id().assign(id, false);
	initiator->get_id().assign(id, true);

	m_active.emplace(id.canonical(), std::move(command));
	m_input.emplace_back(std::move(initiator));
	return id.canonical();
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC initiate_pass
Cmd::IDValue
Context::initiate_pass(
	Cmd::StageUPtr initiator
) noexcept {
	assert(!initiator->is_identified());

	initiator->get_id().assign(next_id(), is_host(), true);
	auto const canonical_id = initiator->get_id_canonical();
	m_output.emplace_back(Dest::remote, std::move(initiator));
	return canonical_id;
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC push_input
void
Context::push_input(
	Cmd::StageUPtr stage
) noexcept {
	assert(stage->is_identified());

	// TODO: Ignore when:
	//   is_local_initiator(stage)
	// (initiate() should've been used instead; also, client
	// is not permitted to generate such a stage).
	m_input.emplace_back(std::move(stage));
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC push_remote
void
Context::push_remote(
	Cmd::Stage const& origin,
	bool const initiator,
	Cmd::StageUPtr stage
) {
	assert( origin.is_identified());
	assert(!stage->is_identified());
	// TODO: Throw if command's type info doesn't have Cmd::Flags::remote?

	auto const it = make_const(m_active).find(origin.get_id_canonical());
	if (m_active.cend() == it) {
		HORD_THROW_FQN(
			ErrorCode::context_output_detached,
			"origin stage does not belong to an active command"
		);
	} else {
		stage->get_id().assign(
			origin.get_id(),
			initiator
		);
		m_output.emplace_back(Dest::remote, std::move(stage));
	}
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC broadcast
void
Context::broadcast(
	Cmd::StageUPtr stage,
	bool const local
) {
	assert(!stage->is_identified());

	m_output.emplace_back(
		local
			? Dest::broadcast_local
			: Dest::broadcast_remote
		,
		std::move(stage)
	);
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC execute_input
std::size_t
Context::execute_input(
	Result& result
) {
	// TODO: logging & more-verbose errors
	result.id = Cmd::ID_NULL;
	result.status = static_cast<Cmd::Status>(0u);
	if (m_input.empty()) {
		return 0u;
	}

	// NB: If the stage is not an initiator (i.e., command is not
	// active), it will be destroyed when this function terminates.
	Cmd::StageUPtr stage_ptr{std::move(m_input.front())};
	Cmd::Stage& stage = *stage_ptr.get();
	m_input.pop_front();

	// NB: Precondition ensured by initiate() and push_input().
	assert(stage.is_identified());

	command_map_type::iterator it = m_active.find(stage.get_id_canonical());
	if (Cmd::Type::GenericTerminate == stage.get_command_type()) {
		result.id = stage.get_id();
		result.status = Cmd::Status::fatal_remote;
		if (m_active.end() != it) {
			it->second->set_status(result.status);
		}
		terminate(stage, it, false);
		goto l_normal_exit;
	}
	if (m_active.end() != it) {
		// Local input (command already constructed)
		if (!it->second->is_active()) {
			if (!stage.is_initiator()) {
				HORD_THROW_FQN(
					ErrorCode::context_execute_not_active,
					"non-initiating stage carries an ID which is"
					" not active (first stage)"
				);
			}
			it->second->get_id().set_initiator(true);
			it->second->get_initiator() = std::move(stage_ptr);
		} else if (stage.is_initiator()) {
			HORD_THROW_FQN(
				ErrorCode::context_execute_already_active,
				"initiating stage carries an ID which is already"
				" active"
			);
		}
	} else if (!stage.is_initiator()) {
		HORD_THROW_FQN(
			ErrorCode::context_execute_not_active,
			"non-initiating stage carries an ID which is"
			" not active"
		);
	} else {
		// Remote input (no existing command; constructed from type)
		auto const&& emplace_result = m_active.emplace(
			stage.get_id_canonical(),
			stage.get_command_type_info().make()
		);
		assert(emplace_result.second); // success
		it = emplace_result.first;
		it->second->get_id() = stage.get_id();
		it->second->get_initiator() = std::move(stage_ptr);
	}

	// execute
	result.id = stage.get_id();
	try {
		result.status = stage.execute(*this, *it->second.get());
	} catch (...) {
		result.status = Cmd::Status::fatal; // implicit
		it->second->set_status(result.status);
		terminate(stage, it, true);
		throw;
	}

	// handle normal result
	switch (result.status) {
	case Cmd::Status::waiting:
		// Do nothing
		break;

	case Cmd::Status::fatal: // fall-through
	case Cmd::Status::fatal_remote:
		terminate(stage, it, true);
		break;

	case Cmd::Status::complete: // fall-through
	case Cmd::Status::error: // fall-through
	case Cmd::Status::error_remote:
		m_done.emplace_back(std::move(it->second));
		m_active.erase(it);
		break;
	}

l_normal_exit:
	return m_input.size();
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS

} // namespace System
} // namespace Hord
