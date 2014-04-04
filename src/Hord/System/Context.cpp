
#include <Hord/utility.hpp>
#include <Hord/System/Driver.hpp>
#include <Hord/System/Context.hpp>
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

void
Context::terminate(
	stage_map_type::iterator it,
	bool const transmitted
) noexcept {
	if (m_active.end() == it) {
		// TODO: Log that remote tried to terminate an inactive
		// command
		return;
	}

	auto const id = it->second->get_id();
	// TODO: Log that an active command is terminating
	// TODO: Log discarded stages
	// Discard pending stage input and output for this command
	for (
		auto in_it = m_input.begin();
		m_input.end() != in_it;
	) {
		if ((*in_it)->get_id() == id) {
			// FIXME: Is this valid in the stdlib?
			m_input.erase(in_it++);
		} else {
			++in_it;
		}
	}
	for (
		auto out_it = m_output.begin();
		m_output.end() != out_it;
	) {
		if ((*out_it)->get_id() == id) {
			// FIXME: Is this valid in the stdlib?
			m_output.erase(out_it++);
		} else {
			++out_it;
		}
	}
	// Push GenericTerminate and inactivate stage
	if (transmitted) {
		auto terminator = Cmd::Generic::Terminate::make_statement();
		push_output(*it->second, terminator, false);
	}
	m_active.erase(it);
}

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
	assert(origin.is_identified());
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
			origin.get_id(),
			is_host(),
			remote_initiator
		);
		m_output.emplace_back(std::move(stage));
	}
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC execute_input
std::size_t
Context::execute_input(
	id_status_pair_type& result
) {
	// TODO: logging & more-verbose errors
	result.first = Cmd::NULL_ID;
	result.second = static_cast<Cmd::Status>(0u);
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
	if (m_active.end() != it) {
		if (stage.is_initiator()) {
			HORD_THROW_FQN(
				ErrorCode::context_execute_already_active,
				"initiating stage carries an ID which is already"
				" active"
			);
		}
	} else if (!stage.is_initiator()) {
		if (Cmd::Type::GenericTerminate != stage.get_command_type()) {
			HORD_THROW_FQN(
				ErrorCode::context_execute_not_active,
				"non-initiating stage carries an ID which is not"
				" active"
			);
		}
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
	result.first = stage.get_id();
	if (Cmd::Type::GenericTerminate == stage.get_command_type()) {
		terminate(it, false);
		result.second = Cmd::Status::error_remote;
	} else {
		try {
			result.second = stage.execute(*this, *it->second.get());
			if (Cmd::Status::error == result.second) {
				terminate(it, !is_local_initiator(stage));
			}
		} catch (...) {
			terminate(it, !is_local_initiator(stage));
			result.second = Cmd::Status::error; // implicit
			throw;
		}
	}
	return m_input.size();
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS

} // namespace System
} // namespace Hord
