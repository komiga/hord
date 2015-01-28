/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Command unit.
@ingroup cmd
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/traits.hpp>
#include <Hord/String.hpp>
#include <Hord/utility.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/System/Driver.hpp>
#include <Hord/System/Context.hpp>

#include <type_traits>
#include <utility>
#include <exception>

namespace Hord {
namespace Cmd {

// Forward declarations
class UnitBase;
template<class>
class Unit;
template<class>
struct has_unit_commit_impl;
template<class, bool const>
struct unit_commit_impl;
template<class>
struct unit_ensure_traits;

/**
	@addtogroup cmd
	@{
*/

/**
	Command implementation boilerplate.

	@note This should be placed within a class definition.

	@remarks Imports base::exec_result_type; defines all special
	member functions, base alias to the Cmd::Unit specialization,
	and the following:
	@remarks
	- <code>static constexpr Cmd::ID const COMMAND_ID;</code>
	- <code>static constexpr char const* command_name();</code>
	- <code>static constexpr Cmd::TypeInfo command_info();</code>

	@param impl_ Implementation class.
	@param name_ Name of command (string literal).
*/
#define HORD_CMD_IMPL_BOILERPLATE(impl_, name_)						\
	private:														\
		using base = ::Hord::Cmd::Unit<impl_>;						\
		using this_type = impl_;									\
		friend base;												\
	public:															\
		using base::exec_result_type;								\
	private:														\
		impl_() = delete;											\
		impl_(impl_ const&) = delete;								\
		impl_& operator=(impl_ const&) = delete;					\
		impl_& operator=(impl_&&) = delete;							\
	public:															\
		~impl_() = default;											\
		impl_(impl_&&) = default;									\
		explicit													\
		impl_(														\
			System::Context& context								\
		) noexcept													\
			: base(context)											\
		{}															\
		static constexpr ::Hord::Cmd::ID const						\
		COMMAND_ID = name_ ## _hash;								\
		static constexpr char const*								\
		command_name() noexcept { return HORD_STR_LIT(name_); }		\
		static constexpr ::Hord::Cmd::TypeInfo						\
		command_info() noexcept {									\
			return {COMMAND_ID, command_name()};					\
		}
//

/**
	Ensure traits for implementation class.

	@pre Must be used within the namespace @c Hord::Cmd.

	@param impl_ Implementation class.
*/
#define HORD_CMD_IMPL_ENSURE_TRAITS(impl_)							\
	template struct ::Hord::Cmd::unit_ensure_traits<impl_>;
//

/**
	Command implementation boilerplate with commit_impl() declaration.

	@sa
		HORD_CMD_IMPL_BOILERPLATE()
*/
#define HORD_CMD_IMPL_BOILERPLATE_WITH_COMMIT(impl_, name_)			\
	HORD_CMD_IMPL_BOILERPLATE(impl_, name_)							\
	private:														\
		void														\
		commit_impl() noexcept
//

/**
	Define command implementation commit_impl().

	@note Function body should follow.

	@param impl_ Implementation class.
*/
#define HORD_CMD_IMPL_COMMIT_DEF(impl_)								\
	void															\
	impl_::commit_impl() noexcept

/**
	Command base class.
*/
class UnitBase {
public:
	/**
		Command execution result type.
	*/
	using exec_result_type = bool;

private:
	using this_type = UnitBase;

	System::Context& m_context;
	Cmd::Result m_result{Cmd::Result::error};
	String m_message{};

protected:
/** @name Common properties */ /// @{
	Hord::Object::ID m_object_id{Hord::Object::ID_NULL};
/// @}

private:
	UnitBase() = delete;
	UnitBase(UnitBase const&) = delete;
	UnitBase& operator=(UnitBase const&) = delete;
	UnitBase& operator=(UnitBase&&) = delete;

protected:
/** @name Internal properties */ /// @{
	/**
		Set result.
	*/
	void
	set_result(
		Cmd::Result const result
	) noexcept {
		m_result = result;
	}

	/**
		Set message.
	*/
	void
	set_message(
		String&& message
	) noexcept {
		m_message = std::move(message);
	}
/// @}

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~UnitBase() = default;

	/** Move constructor. */
	UnitBase(UnitBase&&) = default;

	/**
		Constructor with context.
	*/
	explicit
	UnitBase(
		System::Context& context
	) noexcept
		: m_context(context)
	{}
/// @}

/** @name Properties */ /// @{
	/**
		Get context.
	*/
	System::Context&
	context() noexcept {
		return m_context;
	}

	/**
		Get context.
	*/
	System::Context const&
	context() const noexcept {
		return m_context;
	}

	/**
		Get driver.
	*/
	System::Driver&
	driver() noexcept {
		return context().driver();
	}

	/**
		Get driver.
	*/
	System::Driver const&
	driver() const noexcept {
		return context().driver();
	}

	/**
		Get datastore.
	*/
	IO::Datastore&
	datastore() noexcept {
		return context().datastore();
	}

	/**
		Get datastore.
	*/
	IO::Datastore const&
	datastore() const noexcept {
		return context().datastore();
	}

	/**
		Get error message.
	*/
	String const&
	message() const noexcept {
		return m_message;
	}

	/**
		Get result.
	*/
	Cmd::Result
	result() const noexcept {
		return m_result;
	}

	/**
		Check if command execution succeeded.
	*/
	bool
	ok() const noexcept {
		return !bad();
	}

	/**
		Check if command execution succeeded with action.
	*/
	bool
	ok_action() const noexcept {
		return Cmd::Result::success == m_result;
	}

	/**
		Check if command execution failed.

		@remarks A just-constructed command unit will yield @c true.
	*/
	bool
	bad() const noexcept {
		return Cmd::Result::error == m_result;
	}

	/**
		Get object ID the command acted upon.

		@note This will be Object::ID_NULL for any commands that
		operate on no or multiple objects.
	*/
	Hord::Object::ID
	object_id() const noexcept {
		return m_object_id;
	}
/// @}
};

/**
	Command implementation base class.

	@note Command implementations should derive from this class.

	@tparam Impl Command implementation class.
*/
template<class Impl>
class Unit
	: public Cmd::UnitBase
{
	using base = Cmd::UnitBase;

public:
	/**
		Command execution result type.
	*/
	using exec_result_type = base::exec_result_type;

	/**
		Command implementation class.
	*/
	using impl_type = Impl;

private:
	using this_type = Unit<impl_type>;

	using unit_commit_impl
	= Cmd::unit_commit_impl<
		impl_type,
		Cmd::has_unit_commit_impl<impl_type>::value
	>;

	Unit() = delete;
	Unit(Unit const&) = delete;
	Unit& operator=(Unit const&) = delete;
	Unit& operator=(Unit&&) = delete;

	void
	commit_post() noexcept {
		context().notify_complete(
			*this, impl_type::command_info()
		);
	}

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	~Unit() = default;

	/** Move constructor. */
	Unit(Unit&&) = default;

	/**
		Constructor with context.
	*/
	explicit
	Unit(
		System::Context& context
	) noexcept
		: base(context)
	{}
/// @}

protected:
/** @name Operations */ /// @{
	/**
		Notify exception to context.
	*/
	void
	notify_exception(
		std::exception_ptr eptr
	) noexcept {
		context().notify_exception(
			*this, impl_type::command_info(), std::move(eptr)
		);
	}

	/**
		Notify current exception to context.
	*/
	void
	notify_exception_current() noexcept {
		notify_exception(std::current_exception());
	}

	/**
		Commit command with success.

		@returns @c ok().
	*/
	exec_result_type
	commit() noexcept {
		set_result(Cmd::Result::success);
		unit_commit_impl::func(
			static_cast<impl_type*>(this)
		);
		commit_post();
		return ok();
	}

	/**
		Commit command with error message.

		@returns @c ok().
	*/
	exec_result_type
	commit_error(
		String&& message
	) noexcept {
		set_result(Cmd::Result::error);
		set_message(std::move(message));
		unit_commit_impl::func(
			static_cast<impl_type*>(this)
		);
		commit_post();
		return ok();
	}

	/**
		Commit command with result.

		@note This assumes the error message was already assigned
		if the result is an error.

		@returns @c ok().
	*/
	exec_result_type
	commit_with(
		Cmd::Result const result
	) noexcept {
		set_result(result);
		unit_commit_impl::func(
			static_cast<impl_type*>(this)
		);
		commit_post();
		return ok();
	}
/// @}
};

/** @cond INTERNAL */
template<class Impl>
struct has_unit_commit_impl {
	template<class U>
	static auto match(void(U::*)()) -> U;

	template<class U>
	static auto test(int) -> std::is_same<U, decltype(match(&U::commit_impl))>;
	template<class>
	static auto test(...) -> std::false_type;

	static constexpr bool const
	value = decltype(test<Impl>(0))::value;
};

template<class Impl, bool const = false>
struct unit_commit_impl final {
	static void
	func(
		Impl* const
	) noexcept {
		return;
	}
};

template<class Impl>
struct unit_commit_impl<Impl, true> final {
	static void
	func(
		Impl* const impl
	) noexcept {
		return impl->commit_impl();
	}
};

template<class Impl>
struct unit_ensure_traits
	: traits::require_t<
		Impl,
		tw::capture_post<std::is_base_of, Cmd::Unit<Impl>>::template type,
		std::is_move_constructible,
		std::is_nothrow_destructible
	>
	, traits::disallow_t<
		Impl,
		tw::is_copyable,
		std::is_move_assignable
	>
{};
/** @endcond */ // INTERNAL

/** @} */ // end of doc-group cmd

} // namespace Cmd
} // namespace Hord
