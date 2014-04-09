
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

enum : Cmd::ID {
	BASE_GENID = Cmd::ID{1u}
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
	, m_hive(hive_pair.hive)
	, m_genid(Cmd::NULL_ID)
	, m_active()
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

Cmd::ID
Context::next_id() noexcept {
	if (Cmd::MAX_ID == m_genid) {
		return m_genid = BASE_GENID;
	} else {
		return ++m_genid;
	}
}

#define HORD_SCOPE_FUNC terminate // pseudo
void
Context::purge_id(
	Cmd::ID const id,
	stage_deque_type& deque
) {
	for (
		stage_deque_type::size_type idx = 0;
		deque.size() > idx;
		++idx
	) {
		if (deque[idx]->get_id() == id) {
			Log::acquire()
				<< "discarding stage (command termination): "
				<< *deque[idx]
			;
			deque.erase(deque.cbegin() + idx);
		}
	}
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC terminate
void
Context::terminate(
	Cmd::Stage& input_stage,
	stage_map_type::iterator it,
	bool const push_terminator
) noexcept {
	if (m_active.end() == it) {
		Log::acquire()
			<< "remote terminated inactive command: "
			<< Cmd::IDFieldsPrinter{input_stage}
		;
		return;
	}

	Log::acquire()
		<< "terminating command: "
		<< input_stage
	;
	auto const id = it->second->get_id();
	purge_id(id, m_input);
	purge_id(id, m_output);
	if (push_terminator && !is_local_initiator(input_stage)) {
		auto terminator = Cmd::Generic::Terminate::make_statement();
		terminator->get_id_fields().assign(
			it->second->get_id_fields(),
			false
		);
		m_output.emplace_back(std::move(terminator));
	}
	m_active.erase(it);
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC initiate
void
Context::initiate(
	Cmd::StageUPtr& initiator
) noexcept {
	assert(!initiator->is_identified());

	initiator->get_id_fields().assign(
		next_id(),
		is_host(),
		true
	);
	m_input.emplace_back(std::move(initiator));
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC push_input
void
Context::push_input(
	Cmd::StageUPtr& stage
) noexcept {
	assert(stage->is_identified());

	// TODO: Ignore when:
	//   Type::host == m_type &&
	//   stage->is_host() &&
	//   stage->is_initiator()
	// (initiate() should've been used instead; also, client
	// is not permitted to generate such a stage).
	m_input.emplace_back(std::move(stage));
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC push_output
void
Context::push_output(
	Cmd::Stage const& origin,
	Cmd::StageUPtr& stage,
	bool const remote_initiator
) {
	assert( origin.is_identified());
	assert(!stage->is_identified());
	// TODO: Throw if command's type info doesn't have Cmd::Flags::remote?
	// Is it better to have the userspace handle it, since they're going
	// to handle the output queue anyways? Seems to make sense to allow
	// commands to "output" results locally. Userspace needs to be aware
	// of this so that they don't push non-remote command stages to the
	// remote. Should the "destination" be embedded in the stage?
	// i.e., allow stages to be output for local or remote?
	// Seems more likely that commands won't even output locally
	// if they're Cmd::Flags::remote. I can certainly see a use for having
	// commands that are completely local, though.

	auto const it = make_const(m_active).find(origin.get_id());
	if (m_active.cend() == it) {
		HORD_THROW_FQN(
			ErrorCode::context_output_detached,
			"origin stage does not belong to an active command"
		);
	} else {
		stage->get_id_fields().assign(
			origin.get_id_fields(),
			remote_initiator
		);
		m_output.emplace_back(std::move(stage));
	}
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC push_result
void
Context::push_result(
	Cmd::Stage const& origin,
	Cmd::StageUPtr& stage
) {
	assert(origin.is_identified());

	auto const it = make_const(m_active).find(origin.get_id());
	if (m_active.cend() == it) {
		HORD_THROW_FQN(
			ErrorCode::context_output_detached,
			"origin stage does not belong to an active command"
		);
	}
	if (!stage->is_identified()) {
		stage->get_id_fields().assign(
			origin.get_id_fields(),
			false
		);
	} else {
		assert(stage->get_id() == origin.get_id());
	}
	m_output.emplace_back(std::move(stage));
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC execute_input
std::size_t
Context::execute_input(
	Result& result
) {
	// TODO: logging & more-verbose errors
	result.id = Cmd::NULL_ID;
	result.status = static_cast<Cmd::Status>(0u);
	if (m_input.empty()) {
		return 0u;
	}

	// NB: If the stage is not an initiator (i.e., not emplaced),
	// it will be destroyed when this function terminates.
	Cmd::StageUPtr stage_ptr{std::move(m_input.front())};
	Cmd::Stage& stage = *stage_ptr.get();
	m_input.pop_front();

	// NB: Precondition ensured by initiate() and push_input().
	assert(stage.is_identified());

	stage_map_type::iterator it = m_active.find(stage.get_id());
	if (Cmd::Type::GenericTerminate == stage.get_command_type()) {
		result.id = stage.get_id();
		result.status = Cmd::Status::fatal_remote;
		terminate(stage, it, false);
		goto l_normal_exit;
	}
	if (m_active.end() != it) {
		if (stage.is_initiator()) {
			HORD_THROW_FQN(
				ErrorCode::context_execute_already_active,
				"initiating stage carries an ID which is already"
				" active"
			);
		}
	} else if (!stage.is_initiator()) {
		HORD_THROW_FQN(
			ErrorCode::context_execute_not_active,
			"non-initiating stage carries an ID which is not"
			" active"
		);
	} else {
		// NB: stage is a *ref* to the stage itself, so there's
		// no issue with the uptr move here.
		auto const&& emplace_result = m_active.emplace(
			stage.get_id(),
			std::move(stage_ptr)
		);
		assert(emplace_result.second); // success
		it = emplace_result.first;
	}

	// execute
	result.id = stage.get_id();
	try {
		result.status = stage.execute(*this, *it->second.get());
	} catch (...) {
		terminate(stage, it, true);
		result.status = Cmd::Status::fatal; // implicit
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
