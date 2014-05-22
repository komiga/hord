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
#include <Hord/Cmd/Unit.hpp>
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

	/** Command map. */
	using command_map_type = aux::unordered_map<
		Cmd::IDValue,
		Cmd::UnitUPtr
	>;

	/** Command deque. */
	using command_deque_type = aux::deque<
		Cmd::UnitUPtr
	>;

	/** Command input deque. */
	using input_deque_type = aux::deque<
		Cmd::StageUPtr
	>;

	/**
		%Stage output destinations.
	*/
	enum class Dest : unsigned {
		/** Output to remote endpoint. */
		remote = 1u,
		/** Output to all other endpoints (local initiation). */
		broadcast_local,
		/** Output to all other endpoints (remote initiation). */
		broadcast_remote,
	};

	/**
		Command output.
	*/
	struct Output {
		/** Destination of stage. */
		Context::Dest dest;
		/** %Stage. */
		Cmd::StageUPtr stage;

		/**
			Constructor with destination and stage.
		*/
		Output(
			Context::Dest const dest,
			Cmd::StageUPtr&& stage
		) noexcept
			: dest(dest)
			, stage(std::move(stage))
		{}
	};

	/** Command output deque. */
	using output_deque_type = aux::deque<
		Context::Output
	>;

	/**
		%Stage execution result.
	*/
	struct Result {
		/** Canonical stage %ID. */
		Cmd::IDValue id;
		/** Command status. */
		Cmd::Status status;
	};

private:
	Type const m_type;
	System::Driver& m_driver;
	IO::Datastore& m_datastore;
	Hive::Unit& m_hive;
	Cmd::IDValue m_genid;

	// NB: Stores initiator stages
	command_map_type m_active;

	// Remote results and local results
	command_deque_type m_done;
	input_deque_type m_input;
	output_deque_type m_output;

	Cmd::IDValue
	next_id() noexcept;

	void
	terminate(
		Cmd::Stage& input_stage,
		command_map_type::iterator it,
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
	command_map_type&
	get_active() noexcept {
		return m_active;
	}

	/**
		Get done commands.
	*/
	command_deque_type&
	get_done() noexcept {
		return m_done;
	}

	/**
		Get input stages.
	*/
	input_deque_type&
	get_input() noexcept {
		return m_input;
	}

	/**
		Get output stages.
	*/
	output_deque_type&
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

	/**
		Localize command error status to context.
	*/
	Cmd::Status
	localized_error(
		Cmd::Stage const& stage
	) {
		return is_local(stage)
			? Cmd::Status::error
			: Cmd::Status::error_remote
		;
	}

	/**
		Localize command error status to context.
	*/
	Cmd::Status
	localized_fatal(
		Cmd::Stage const& stage
	) {
		return is_local(stage)
			? Cmd::Status::fatal
			: Cmd::Status::fatal_remote
		;
	}
/// @}

/** @name Operations */ /// @{
	/**
		Initiate command.

		@note This will push and initiate a command as input. The
		command will not be active until the initiator stage is
		executed from execute_input().

		@pre @code
			!command ->is_identified() &&
			!command->has_initiator() &&
			!initiate->is_identified() &&
			initiator->get_command_type() == command->get_type()
		@endcode
	*/
	void
	initiate(
		Cmd::UnitUPtr command,
		Cmd::StageUPtr initiator
	) noexcept;

	/**
		Initiate command without making it active locally.

		@note This will push and initiate a command as output. This
		is used for remote broadcasts and, in general, initiator
		stages that don't need to execute anything locally and don't
		require a response from the remote endpoint. It is
		conceptually equivalent to having the stage's execution
		immediately return Cmd::Status::complete (but it is never
		executed; the stage only takes the next local ID and is
		pushed to the remote endpoint).

		@post No active nor done command for the stage on the local
		endpoint.

		@pre @code
			!initiate->is_identified()
		@endcode
	*/
	void
	initiate_pass(
		Cmd::StageUPtr initiator
	) noexcept;

	/**
		Push command input.

		@note This is used by userspace to emplace remote stages.

		@pre @code stage->is_identified() @endcode
	*/
	void
	push_input(
		Cmd::StageUPtr stage
	) noexcept;

	/**
		Push stage to remote endpoint.

		@pre @code
			 origin->is_identified() &&
			!stage ->is_identified()
		@endcode

		@throws Error{ErrorCode::context_output_detached}
		If @a origin is not part of an active command.

		@param origin Origin stage.
		@param initiator Whether the remote endpoint should emplace
		the stage as an initiator (see @c Cmd::ID).
		@param stage %Stage to push.
	*/
	void
	push_remote(
		Cmd::Stage const& origin,
		bool const initiator,
		Cmd::StageUPtr stage
	);

	/**
		Broadcast stage to all other contexts.

		@note @a stage will carry no identity in the output queue.
		Userspace should broadcast a clone of the stage by either
		calling initiate() (when local) or initiate_pass() (when
		remote) on every <strong>other</strong> context.

		@par
		@note If @a stage is pushed, the context takes ownership
		(@a stage is moved). Otherwise, the callee retains ownership.

		@pre @code
			!stage->is_identified()
		@endcode

		@param stage %Stage to broadcast.
		@param local Whether the stage should be initiated locally
		(or remotely).
	*/
	void
	broadcast(
		Cmd::StageUPtr stage,
		bool const local
	);

	/**
		Execute command on the context.

		@par
		@note Commands are removed (and destroyed) from the active group
		when execution status is anything other than
		Cmd::Status::waiting.

		@note
		If the stage returns a fatal error or throws an exception,
		the local instance is terminated, existing input and output
		are removed, and GenericTerminate is emitted to the remote,
		which does the same termination, sans GenericTerminate
		emittance.

		@note
		If GenericTerminate is received as input, the result status
		is Cmd::Status::fatal_remote.

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
		@param[out] result Execution result.
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
