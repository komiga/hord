/**
@file Cmd/Unit.hpp
@brief Command unit.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/traits.hpp>
#include <Hord/utility.hpp>
#include <Hord/String.hpp>
#include <Hord/Hive/Unit.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/System/Driver.hpp>
#include <Hord/System/Context.hpp>

#include <am/hash/murmur.hpp>

#include <type_traits>
#include <utility>

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

	@remarks Defines base to the Cmd::Unit specialization, imports
	base::exec_result_type, and defines all special member functions.

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
		static constexpr ::Hord::Cmd::type_info						\
		command_info() noexcept {									\
			return {												\
				::am::hash::murmur3_c<::Hord::Cmd::ID_HASH_LENGTH>(	\
					HORD_STR_LIT(name_),							\
					sizeof(HORD_STR_LIT(name_)),					\
					::Hord::Cmd::ID_HASH_SEED						\
				),													\
				HORD_STR_LIT(name_)									\
			};														\
		}															\
		static constexpr char const*								\
		command_name() noexcept { return HORD_STR_LIT(name_); }
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

/** @cond INTERNAL */
	void
	base_notify_complete(
		Cmd::type_info const& type_info
	) noexcept {
		get_context().notify_complete(*this, type_info);
	}
/** @endcond */ // INTERNAL

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
	get_context() noexcept {
		return m_context;
	}

	/**
		Get context.
	*/
	System::Context const&
	get_context() const noexcept {
		return m_context;
	}

	/**
		Get driver.
	*/
	System::Driver&
	get_driver() noexcept {
		return get_context().get_driver();
	}

	/**
		Get driver.
	*/
	System::Driver const&
	get_driver() const noexcept {
		return get_context().get_driver();
	}

	/**
		Get datastore.
	*/
	IO::Datastore&
	get_datastore() noexcept {
		return get_context().get_datastore();
	}

	/**
		Get datastore.
	*/
	IO::Datastore const&
	get_datastore() const noexcept {
		return get_context().get_datastore();
	}

	/**
		Get hive.
	*/
	Hive::Unit&
	get_hive() noexcept {
		return get_context().get_hive();
	}

	/**
		Get hive.
	*/
	Hive::Unit const&
	get_hive() const noexcept {
		return get_context().get_hive();
	}

	/**
		Get error message.
	*/
	String const&
	get_message() const noexcept {
		return m_message;
	}

	/**
		Check if command execution succeeded.
	*/
	bool
	ok() const noexcept {
		return !bad();
	}

	/**
		Check if command execution failed.

		@remarks A just-constructed command unit will yield @c true.
	*/
	bool
	bad() const noexcept {
		return m_result == Cmd::Result::error;
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
		Commit command with success.

		@returns @c ok().
	*/
	exec_result_type
	commit() noexcept {
		set_result(Cmd::Result::success);
		unit_commit_impl::func(
			static_cast<impl_type*>(this)
		);
		base_notify_complete(impl_type::command_info());
		return ok();
	}

	/**
		Commit command with error message.

		@returns @c ok().
	*/
	exec_result_type
	commit(
		String&& message
	) noexcept {
		set_result(Cmd::Result::error);
		set_message(std::move(message));
		unit_commit_impl::func(
			static_cast<impl_type*>(this)
		);
		base_notify_complete(impl_type::command_info());
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
		base_notify_complete(impl_type::command_info());
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
