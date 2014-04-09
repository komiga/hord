/**
@file System/Context.hpp
@brief Context class.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_SYSTEM_CONTEXT_HPP_
#define HORD_SYSTEM_CONTEXT_HPP_

#include <Hord/config.hpp>
#include <Hord/aux.hpp>
#include <Hord/Hive/Defs.hpp>
#include <Hord/Hive/Unit.hpp>
#include <Hord/IO/Datastore.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/Stage.hpp>
#include <Hord/System/Driver.hpp>

#include <utility>

namespace Hord {
namespace System {

// Forward declarations
class Context;

/**
	@addtogroup system
	@{
*/

/**
	%Context.

	@warning The context requires the attached datastore and hive
	exist as long as the context exists.

	@remarks This class mediates the behavior of commands one layer
	above the command transmission protocol for a server or client.
*/
class Context final {
public:
	/**
		%Context type.
	*/
	enum class Type : unsigned {
		/**
			Host view.
		*/
		host,

		/**
			Client view.
		*/
		client
	};

	/** Command stage map. */
	using stage_map_type = aux::unordered_map<
		Cmd::ID,
		Cmd::StageUPtr
	>;

	/** Command stage deque. */
	using stage_deque_type = aux::deque<
		Cmd::StageUPtr
	>;

	/**
		Stage execution result.
	*/
	struct Result {
		/** Canonical stage %ID. */
		Cmd::ID id;
		/** Command status. */
		Cmd::Status status;
	};

private:
	Type const m_type;
	System::Driver& m_driver;
	IO::Datastore& m_datastore;
	Hive::Unit& m_hive;
	Cmd::ID m_genid;

	// NB: Stores initiator stages
	stage_map_type m_active;

	// Remote results and local results
	stage_deque_type m_results;
	stage_deque_type m_input;
	stage_deque_type m_output;

	Cmd::ID
	next_id() noexcept;

	static void
	purge_id(
		Cmd::ID const id,
		stage_deque_type& deque
	);

	void
	terminate(
		Cmd::Stage& input_stage,
		stage_map_type::iterator it,
		bool const push_terminator
	) noexcept;

	Context() = delete;
	Context(Context const&) = delete;
	Context& operator=(Context const&) = delete;
	Context& operator=(Context&&) = delete;

	Context(
		Type const type,
		System::Driver& driver,
		System::Driver::datastore_hive_pair& hive_pair
	);

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~Context() noexcept;

	/** Move constructor. */
	Context(Context&&) noexcept;

	/**
		Constructor with type, driver, and hive ID.

		@throws Error{ErrorCode::context_invalid_hive}
		If @a hive_id does not exist within @a driver.

		@param type %Type.
		@param driver %Driver.
		@param hive_id %Hive ID.
	*/
	Context(
		Type const type,
		System::Driver& driver,
		Hive::ID const hive_id
	);
/// @}

/** @name Properties */ /// @{
	/**
		Get type.
	*/
	Type
	get_type() const noexcept {
		return m_type;
	}

	/**
		Get driver.
	*/
	System::Driver&
	get_driver() noexcept {
		return m_driver;
	}

	/**
		Get driver.
	*/
	System::Driver const&
	get_driver() const noexcept {
		return m_driver;
	}

	/**
		Get datastore.
	*/
	IO::Datastore&
	get_datastore() noexcept {
		return m_datastore;
	}

	/**
		Get datastore.
	*/
	IO::Datastore const&
	get_datastore() const noexcept {
		return m_datastore;
	}

	/**
		Get hive (mutable).
	*/
	Hive::Unit&
	get_hive() noexcept {
		return m_hive;
	}

	/**
		Get hive.
	*/
	Hive::Unit const&
	get_hive() const noexcept {
		return m_hive;
	}

	/**
		Get active commands.
	*/
	stage_map_type&
	get_active() noexcept {
		return m_active;
	}

	/**
		Get command results.
	*/
	stage_deque_type&
	get_results() noexcept {
		return m_results;
	}

	/**
		Get input stages.
	*/
	stage_deque_type&
	get_input() noexcept {
		return m_input;
	}

	/**
		Get output stages.
	*/
	stage_deque_type&
	get_output() noexcept {
		return m_output;
	}

	/**
		Check if context is a host.
	*/
	bool
	is_host() const noexcept {
		return Context::Type::host == m_type;
	}

	/**
		Check if context is a client.
	*/
	bool
	is_client() const noexcept {
		return Context::Type::client == m_type;
	}

	/**
		Check if stage is local to the context.
	*/
	bool
	is_local(
		Cmd::Stage const& stage
	) const noexcept {
		return is_host()
			? stage.is_host()
			: stage.is_client()
		;
	}

	/**
		Check if stage is remote to the context.
	*/
	bool
	is_remote(
		Cmd::Stage const& stage
	) const noexcept {
		return !is_local(stage);
	}

	/**
		Check if stage is a context-local initiator.
	*/
	bool
	is_local_initiator(
		Cmd::Stage const& stage
	) const noexcept {
		return is_local(stage) && stage.is_initiator();
	}

	/**
		Check if stage is a context-remote initiator.
	*/
	bool
	is_remote_initiator(
		Cmd::Stage const& stage
	) const noexcept {
		return is_remote(stage) && stage.is_initiator();
	}
/// @}

/** @name Operations */ /// @{
	/**
		Initiate command.

		@note This will push an input stage. The command will not
		be active until the initiator stage is executed from
		execute_input().

		@par
		@note If @a initiator is pushed, the context takes ownership
		(@a initiator is moved). Otherwise, the callee retains
		ownership.

		@pre @code !stage->is_identified() @endcode

		@param initiator Initiator stage.
	*/
	void
	initiate(
		Cmd::StageUPtr& initiator
	) noexcept;

	/**
		Push command input.

		@note This is used by userspace to emplace remote stages.

		@par
		@note If @a stage is pushed, the context takes ownership
		(@a stage is moved). Otherwise, the callee retains ownership.

		@pre @code stage->is_identified() @endcode

		@param stage %Stage to push.
	*/
	void
	push_input(
		Cmd::StageUPtr& stage
	) noexcept;

	/**
		Push command output.

		@note This is only used by command stages to emplace stage
		output to the remote endpoint.

		@par
		@note If @a stage is pushed, the context takes ownership
		(@a stage is moved). Otherwise, the callee retains ownership.

		@pre @code
			 origin->is_identified() &&
			!stage ->is_identified()
		@endcode

		@throws Error{ErrorCode::context_output_detached}
		If @a origin is not part of an active command.

		@param origin Origin stage.
		@param stage %Stage to push.
		@param remote_initiator Whether the remote endpoint should
		emplace the stage as an initiator (see @c Cmd::IDFields).
	*/
	void
	push_output(
		Cmd::Stage const& origin,
		Cmd::StageUPtr& stage,
		bool const remote_initiator
	);

	/**
		Push command result.

		@note This is only used by command stages to emplace stage
		results to the local endpoint.

		@par
		@warning If @a stage is already identified, it must have the
		same canonical ID as @a origin.

		@par
		@note If @a stage is pushed, the context takes ownership
		(@a stage is moved). Otherwise, the callee retains ownership.

		@pre @code
			origin->is_identified() &&
			(
				!stage->is_identified() ||
				stage->get_id() == origin->get_id()
			)
		@endcode

		@throws Error{ErrorCode::context_output_detached}
		If @a origin is not part of an active command.

		@param origin Origin stage.
		@param stage %Stage to push.
	*/
	void
	push_result(
		Cmd::Stage const& origin,
		Cmd::StageUPtr& stage
	);

	/**
		Execute command on the context.

		@par
		@note Commands are terminated (i.e., removed from the active
		collection) when a stage returns a status other than
		Cmd::Status::waiting.

		@note
		If the stage returns Cmd::Status::complete, termination just
		removes the initiator stage from the active collection.

		@note
		If the stage returns an error or throws an exception, the
		local instance is terminated, existing input and output is
		removed, and GenericTerminate is emitted to the remote, which
		does the same termination, sans GenericTerminate emittance.

		@note
		If GenericTerminate is received as input, the result status
		is Cmd::Status::error_remote. Local errors or exceptions
		should have Cmd::Status::error.

		@par
		@note If there are no stages to execute, @a result holds
		@c Cmd::NULL_ID and @c static_cast<Cmd::Status>(0u) (an
		invalid status).

		@throws Error{ErrorCode::context_execute_not_active}
		If a non-initiating input stage carries an inactive %ID.

		@throws Error{ErrorCode::context_execute_already_active}
		If an initiating input stage carries an active %ID.

		@throws ...
		Exception from the stage.

		@returns The number of remaining input stages.
		@param result Execution result.
	*/
	std::size_t
	execute_input(
		Result& result
	);
/// @}
};

/** @} */ // end of doc-group system

} // namespace System
} // namespace Hord

#endif // HORD_SYSTEM_CONTEXT_HPP_
