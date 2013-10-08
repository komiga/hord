/**
@file System/Context.hpp
@brief Context class.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_SYSTEM_CONTEXT_HPP_
#define HORD_SYSTEM_CONTEXT_HPP_

#include <Hord/config.hpp>
#include <Hord/aux.hpp>
#include <Hord/Hive/Defs.hpp>
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

	/** Command stage vector. */
	using stage_vector_type = aux::vector<
		Cmd::StageUPtr
	>;

	/** Command %ID and status pair. */
	using id_status_pair_type = std::pair<
		Cmd::ID,
		Cmd::Status
	>;

private:
	Type const m_type;
	System::Driver& m_driver;
	Hive::ID const m_hive_id;
	Cmd::ID m_genid;

	// NB: Stores initiator stages
	stage_map_type m_active;

	// Remote results and local results
	stage_vector_type m_input;
	stage_vector_type m_output;

	Cmd::ID
	next_id() noexcept;

	void
	terminate(
		Cmd::ID const id
	) noexcept;

	Context() = delete;
	Context(Context const&) = delete;
	Context& operator=(Context const&) = delete;
	Context& operator=(Context&&) = delete;

public:
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with type, driver, and hive ID.

		@param type %Type.
		@param driver %Driver.
		@param hive_id %Hive ID.
	*/
	Context(
		Type const type,
		System::Driver& driver,
		Hive::ID const hive_id
	) noexcept;

	/** Move constructor. */
	Context(Context&&) noexcept;
	/** Destructor. */
	~Context() noexcept;
/// @}

/** @name Operations */ /// @{
	/**
		Get type.
	*/
	Type
	get_type() const noexcept {
		return m_type;
	}

	/**
		Get driver.
		@{
	*/
	Driver&
	get_driver() noexcept {
		return m_driver;
	}
	Driver const&
	get_driver() const noexcept {
		return m_driver;
	}
	/** @} */

	/**
		Get hive ID.
	*/
	Hive::ID
	get_hive_id() const noexcept {
		return m_hive_id;
	}

	/**
		Get active commands.
	*/
	stage_map_type&
	get_active() noexcept {
		return m_active;
	}

	/**
		Get input stages.
	*/
	stage_vector_type&
	get_input() noexcept {
		return m_input;
	}

	/**
		Get output stages.
	*/
	stage_vector_type&
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
/// @}

/** @name Collections */ /// @{
	/**
		Get the context's datastore-hive pair.

		@throws Error{ErrorCode::context_invalid_hive}
		If context's hive ID is not longer valid.
	*/
	Driver::hive_datastore_pair_type&
	get_hive_pair();
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
		(@a stage is moved). Otherwise, the callee retains
		ownership.

		@pre @code stage->is_identified() @endcode

		@param stage %Stage to push.
	*/
	void
	push_input(
		Cmd::StageUPtr& stage
	) noexcept;

	/**
		Push command output.

		@note This is only used by command stages to emplace local
		results.

		@par
		@note If @a stage is pushed, the context takes ownership
		(@a stage is moved). Otherwise, the callee retains
		ownership.

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
		Execute command on the context.

		@note Before an exception is rethrown,
		-# The local command will be terminated;
		-# @a result will be assigned to the offending command %ID
		   and @c Cmd::Status::error; and
		-# a @c GenericTerminate stage will be emitted if either the
		   command is active (i.e., the stage is not a local
		   initiator) or the remote command is waiting for a result.

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
		id_status_pair_type& result
	);
/// @}
};

/** @} */ // end of doc-group system

} // namespace System
} // namespace Hord

#endif // HORD_SYSTEM_CONTEXT_HPP_
