/**
@file IO/Datastore.hpp
@brief Datastore class.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_IO_DATASTORE_HPP_
#define HORD_IO_DATASTORE_HPP_

#include <Hord/config.hpp>
#include <Hord/traits.hpp>
#include <Hord/String.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/Prop.hpp>

#include <duct/StateStore.hpp>

#include <cassert>
#include <functional>
#include <iosfwd>

namespace Hord {
namespace IO {

// Forward declarations
class Datastore;

/**
	@addtogroup io
	@{
*/
/**
	@addtogroup datastore
	@{
*/

/**
	Base datastore.

	This is the data interface for hives.

	@note %Datastores must be locked when a single prop stream is
	active.
*/
class Datastore {
public:
	/**
		Type info.
	*/
	struct type_info final {
		/**
			Construct a datastore of this type.

			@returns
			- The constructed datastore; or
			- @c nullptr if construction failed.
			@param root_path Root path.
		*/
		Datastore*
		(&construct)(
			String root_path
		) noexcept;
	};

	/**
		Ensure traits for deriving classes.

		@remarks All constructors should be hidden or deleted.

		@tparam D Deriving class.
	*/
	template<
		typename D
	>
	struct ensure_traits :
		traits::require_t<
			D,
			tw::capture_post<std::is_base_of, Datastore>::type,
			// FIXME: libstdc++ 4.7.3 doesn't have the nothrow version
			std::is_destructible
		>,
		traits::disallow_t<
			D,
			std::is_default_constructible,
			tw::capture<std::is_constructible, String>::type,
			tw::is_fully_copyable,
			tw::is_fully_moveable
		>
	{};

protected:
/** @name Implementation */ /// @{
	/**
		open() implementation.
	*/
	virtual void
	open_impl() = 0;

	/**
		close() implementation.

		@remarks This is not called if @c is_open()==false.
	*/
	virtual void
	close_impl() = 0;

	/**
		acquire_input_stream() implementation.
	*/
	virtual std::istream&
	acquire_input_stream_impl(
		IO::PropInfo const& prop_info
	) = 0;

	/**
		acquire_output_stream() implementation.
	*/
	virtual std::ostream&
	acquire_output_stream_impl(
		IO::PropInfo const& prop_info
	) = 0;

	/**
		release_input_stream() implementation.
	*/
	virtual void
	release_input_stream_impl(
		IO::PropInfo const& prop_info
	) = 0;

	/**
		release_output_stream() implementation.
	*/
	virtual void
	release_output_stream_impl(
		IO::PropInfo const& prop_info
	) = 0;
/// @}

/** @name Internal state */ /// @{
	/**
		States.

		Implementations are permitted to define states @c 1<<8
		to @c 1<<31.
	*/
	enum class State : uint32_t {
		/** %Datastore is open. */
		opened = 1 << 0,
		/** %Datastore is locked. */
		locked = 1 << 1,

		/** First reserved state. */
		RESERVED_FIRST = 1 << 2,
		/** Last reserved state. */
		RESERVED_LAST = 1 << 7
	};

/** @cond INTERNAL */
#define HORD_STATE_ASSERT_VALID__(x__)	\
	assert(								\
		State::RESERVED_FIRST > x__ &&	\
		State::RESERVED_LAST  < x__		\
	);
/** @endcond */

	/**
		Enable state.

		@param state %State to enable.
	*/
	/*constexpr*/ void
	enable_state(
		State const state
	) noexcept {
		HORD_STATE_ASSERT_VALID__(state);
		m_states.enable(state);
	}

	/**
		Disable state.

		@param state %State to disable.
	*/
	/*constexpr*/ void
	disable_state(
		State const state
	) noexcept {
		HORD_STATE_ASSERT_VALID__(state);
		m_states.disable(state);
	}

	/**
		Enable or disable state.

		@param state %State to enable or disable.
		@param enable Whether to enable or disable the state.
	*/
	/*constexpr*/ void
	set_state(
		State const state,
		bool const enable
	) noexcept {
		HORD_STATE_ASSERT_VALID__(state);
		m_states.set(state, enable);
	}

	/**
		Test value of state.

		@returns
		- @c true if the state is enabled;
		- @c false if the state is disabled.
		@param state State to test.
	*/
	/*constexpr*/ bool
	test_state(
		State const state
	) const noexcept {
		HORD_STATE_ASSERT_VALID__(state);
		return m_states.test(state);
	}
#undef HORD_STATE_ASSERT_VALID__
/// @}

private:
	duct::StateStore<State> m_states;
	String m_root_path;

	Datastore() = delete;
	Datastore(Datastore const&) = delete;
	Datastore(Datastore&&) = delete;
	Datastore& operator=(Datastore const&) = delete;
	Datastore& operator=(Datastore&&) = delete;

protected:
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with root path.

		@param root_path Root path.
	*/
	Datastore(
		String root_path
	) noexcept;
public:
	/** Destructor. */
	virtual
	~Datastore() = 0;
/// @}

public:
/** @name Properties */ /// @{
	/**
		Set root path.

		@throws Error{ErrorCode::datastore_property_immutable}
		If the datastore is open.

		@param root_path New root path.
	*/
	void
	set_root_path(
		String root_path
	);

	/**
		Get root path.

		@returns Root path.
	*/
	String const&
	get_root_path() const noexcept {
		return m_root_path;
	}

	/**
		Check if the datastore is open.

		@returns
		- @c true if the datastore is open;
		- @c false if it is closed.
	*/
	bool
	is_open() const noexcept {
		return test_state(State::opened);
	}

	/**
		Check if the datastore is locked.

		@returns
		- @c true if the datastore is locked;
		- @c false if it is not locked.
	*/
	bool
	is_locked() const noexcept {
		return test_state(State::locked);
	}
/// @}

/** @name Operations */ /// @{
	/**
		Open the datastore.

		@throws Error{ErrorCode::datastore_open_already}
		If the datastore is already open.

		@throws Error{ErrorCode::datastore_open_failed}
		If the datastore failed to open.
	*/
	void
	open();

	/**
		Close the datastore.

		@throws Error{ErrorCode::datastore_locked}
		If the datastore is locked.
	*/
	void
	close();

	/**
		Acquire raw stream for prop.

		@throws Error{ErrorCode::datastore_closed}
		If the datastore is closed.

		@throws Error{ErrorCode::datastore_locked}
		If the datastore is locked.

		@throws Error{ErrorCode::datastore_object_not_found}
		If @c prop_info.object_id does not exist in the datastore.

		@throws Error{ErrorCode::datastore_prop_unsupplied}
		If the object for @c prop_info.object_id does not supply the
		requested prop.

		@throws Error{..}
		<em>Implementation-defined exceptions.</em>

		@returns Raw prop stream.
		@param prop_info Prop info.

		@sa IO::PropInfo,
			IO::InputPropStream,
			IO::OutputPropStream
	*/
	std::istream&
	acquire_input_stream(
		IO::PropInfo const& prop_info
	);

	/** @copydoc acquire_input_stream(IO::PropInfo const&) */
	std::ostream&
	acquire_output_stream(
		IO::PropInfo const& prop_info
	);

	/**
		Release raw stream for prop.

		@throws Error{ErrorCode::datastore_closed}
		If the datastore is closed.

		@throws Error{ErrorCode::datastore_prop_not_locked}
		If either @a prop_info does not match the currently locked
		prop stream or there is no currently locked prop stream.

		@throws Error{ErrorCode::datastore_object_not_found}
		If @c prop_info.object_id does not exist in the datastore.

		@throws Error{ErrorCode::datastore_prop_unsupplied}
		If the object for @c prop_info.object_id does not supply
		the requested prop.

		@throws Error{..}
		<em>Implementation-defined exceptions.</em>

		@param prop_info Prop info.

		@sa IO::PropInfo,
			IO::InputPropStream,
			IO::OutputPropStream
	*/
	void
	release_input_stream(
		IO::PropInfo const& prop_info
	);

	/** @copydoc release_input_stream(IO::PropInfo const&) */
	void
	release_output_stream(
		IO::PropInfo const& prop_info
	);
/// @}
};

/** @} */ // end of doc-group datastore
/** @} */ // end of doc-group io

} // namespace IO

template struct traits::require_t<
	IO::Datastore,
	std::has_virtual_destructor
>;

} // namespace Hord

#endif // HORD_IO_DATASTORE_HPP_