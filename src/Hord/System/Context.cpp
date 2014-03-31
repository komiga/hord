
#include <Hord/utility.hpp>
#include <Hord/System/Driver.hpp>
#include <Hord/System/Context.hpp>

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

Context::Context(Context&&) noexcept = default;
Context::~Context() noexcept = default;

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
	Cmd::ID const /*id*/
) noexcept {
	// TODO
}

void
Context::initiate(
	Cmd::StageUPtr& initiator
) noexcept {
	assert(!initiator->is_identified());

	initiator->get_id_fields().assign(
		next_id(),
		(Type::host == m_type),
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

	auto const it = make_const(m_active).find(origin.get_id());
	if (m_active.cend() == it) {
		HORD_THROW_FQN(
			ErrorCode::context_output_detached,
			"origin stage does not belong to an active command"
		);
	} else {
		stage->get_id_fields().assign(
			origin.get_id(),
			(Type::host == m_type),
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
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS

} // namespace System
} // namespace Hord
