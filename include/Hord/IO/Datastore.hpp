/**
@file IO/Datastore.hpp
@brief Datastore class.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_IO_DATASTORE_HPP_
#define HORD_IO_DATASTORE_HPP_

#include <Hord/config.hpp>
#include <Hord/traits.hpp>
#include <Hord/String.hpp>
#include <Hord/System/IDGenerator.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/Prop.hpp>
#include <Hord/IO/StorageInfo.hpp>
#include <Hord/Object/Defs.hpp>

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

	@par
	@note Object::NULL_ID represents the hive within the datastore.
	Thus, a prop requested for %NULL_ID is valid.
*/
class Datastore {
public:
	/**
		Type info.
	*/
	struct type_info final {
	/** @name Operations */ /// @{
		// TODO: Add UPtr alias, return UPtr instead
		/**
			Construct a datastore of this type.

			@returns
			- The constructed datastore; or
			- @c nullptr if construction failed.
			@param root_path Root path.
		*/
		IO::Datastore*
		(&construct)(
			String root_path
		) noexcept;
	/// @}
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
			std::is_nothrow_destructible
		>,
		traits::disallow_t<
			D,
			std::is_default_constructible,
			tw::capture<std::is_constructible, String>::type,
			tw::is_copyable,
			tw::is_moveable
		>
	{};

	/**
		Storage information map.
	*/
	using storage_info_map_type
	= aux::unordered_map<
		Object::ID,
		IO::StorageInfo
	>;

protected:
/** @name Implementation */ /// @{
	/**
		open() implementation.

		@throws Error{ErrorCode::datastore_open_failed}
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

		@throws Error{ErrorCode::datastore_object_not_found}
		@throws Error{ErrorCode::datastore_prop_void}
		@throws Error{ErrorCode::datastore_prop_unsupplied}
		@throws Error{..}
	*/
	virtual std::istream&
	acquire_input_stream_impl(
		IO::PropInfo const& prop_info
	) = 0;

	/**
		acquire_output_stream() implementation.

		@throws Error{ErrorCode::datastore_object_not_found}
		@throws Error{ErrorCode::datastore_prop_void}
		@throws Error{ErrorCode::datastore_prop_unsupplied}
		@throws Error{..}
	*/
	virtual std::ostream&
	acquire_output_stream_impl(
		IO::PropInfo const& prop_info
	) = 0;

	/**
		release_input_stream() implementation.

		@note This will not be called if @c is_locked()==false.
		The @c datastore_prop_not_locked exception shall be used for
		other implementation-specific reasons.

		@throws Error{ErrorCode::datastore_prop_not_locked}
		@throws Error{ErrorCode::datastore_object_not_found}
		@throws Error{..}
	*/
	virtual void
	release_input_stream_impl(
		IO::PropInfo const& prop_info
	) = 0;

	/**
		release_output_stream() implementation.

		@note This will not be called if @c is_locked()==false.
		The @c datastore_prop_not_locked exception shall be used for
		other implementation-specific reasons.

		@throws Error{ErrorCode::datastore_prop_not_locked}
		@throws Error{ErrorCode::datastore_object_not_found}
		@throws Error{..}
	*/
	virtual void
	release_output_stream_impl(
		IO::PropInfo const& prop_info
	) = 0;

	/**
		generate_id() implementation.

		@remarks The implementation should generate a unique ID
		within the entire ID set of the datastore -- i.e., including
		orphaned objects (such as trash).
	*/
	virtual Object::ID
	generate_id_impl(
		System::IDGenerator& generator
	) const noexcept = 0;

	/**
		create_object() implementation.

		@throws Error{ErrorCode::datastore_object_already_exists}
		@throws Error{..}
	*/
	virtual void
	create_object_impl(
		Object::ID const object_id,
		Object::type_info const& type_info
	) = 0;

	/**
		destroy_object() implementation.

		@throws Error{ErrorCode::datastore_object_not_found}
		@throws Error{..}
	*/
	virtual void
	destroy_object_impl(
		Object::ID const object_id
	) = 0;
/// @}

/** @name Internal state */ /// @{
	/**
		States.

		Implementations are permitted to define states @c 1<<8
		to @c 1<<31.
	*/
	enum class State : std::uint32_t {
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
#define HORD_STATE_ASSERT_VALID_(x_)	\
	assert(								\
		State::RESERVED_FIRST > x_ &&	\
		State::RESERVED_LAST  < x_		\
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
		HORD_STATE_ASSERT_VALID_(state);
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
		HORD_STATE_ASSERT_VALID_(state);
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
		HORD_STATE_ASSERT_VALID_(state);
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
		HORD_STATE_ASSERT_VALID_(state);
		return m_states.test(state);
	}
#undef HORD_STATE_ASSERT_VALID_
/// @}

private:
	duct::StateStore<State> m_states;
	String m_root_path;
	storage_info_map_type m_storage_info;

	Datastore() = delete;
	Datastore(Datastore const&) = delete;
	Datastore(Datastore&&) = delete;
	Datastore& operator=(Datastore const&) = delete;
	Datastore& operator=(Datastore&&) = delete;

/** @name Special member functions */ /// @{
public:
	/** Destructor. */
	virtual
	~Datastore() = 0;

protected:
	/**
		Constructor with root path.

		@param root_path Root path.
	*/
	Datastore(
		String root_path
	) noexcept;
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
	*/
	String const&
	get_root_path() const noexcept {
		return m_root_path;
	}

	/**
		Check if the datastore is open.
	*/
	bool
	is_open() const noexcept {
		return test_state(State::opened);
	}

	/**
		Check if the datastore is locked.
	*/
	bool
	is_locked() const noexcept {
		return test_state(State::locked);
	}

protected:
	/**
		Get storage info (mutable).
	*/
	storage_info_map_type&
	get_storage_info() noexcept {
		return m_storage_info;
	}

public:
	/**
		Get storage info.
	*/
	storage_info_map_type const&
	get_storage_info() const noexcept {
		return m_storage_info;
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

		@throws Error{ErrorCode::datastore_prop_void}
		If the object for @c prop_info.object_id has not yet created
		the prop (but it is otherwise valid). This can only occur
		when acquiring an input stream.

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
		prop stream or there is currently no locked prop stream.

		@throws Error{ErrorCode::datastore_object_not_found}
		If @c prop_info.object_id does not exist in the datastore.

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

/** @name Objects */ /// @{
	/**
		Generate a %Hive-unique ID.

		@throws Error{ErrorCode::datastore_closed}
		If the datastore is closed.

		@returns The generated object ID.
		@param generator ID generator.
	*/
	Object::ID
	generate_id(
		System::IDGenerator& generator
	) const;

	/**
		Create an object in the datastore.

		@throws Error{ErrorCode::datastore_object_type_prohibited}
		If <code>object_type != Object::Type::Node</code>, which is
		currently the only object type that can be created.

		@throws Error{ErrorCode::datastore_closed}
		If the datastore is closed.

		@throws Error{ErrorCode::datastore_locked}
		If the datastore is locked.

		@throws Error{ErrorCode::datastore_object_already_exists}
		If @a object_id already exists in the datastore.

		@throws Error{..}
		<em>Implementation-defined exceptions.</em>

		@param object_id ID of object.
		@param type_info Object type info.
	*/
	void
	create_object(
		Object::ID const object_id,
		Object::type_info const& type_info
	);

	/**
		Destroy an object in the datastore.

		@throws Error{ErrorCode::datastore_closed}
		If the datastore is closed.

		@throws Error{ErrorCode::datastore_locked}
		If the datastore is locked.

		@throws Error{ErrorCode::datastore_object_not_found}
		If @a object_id does not exist in the datastore.

		@throws Error{..}
		<em>Implementation-defined exceptions.</em>

		@param object_id ID of object.
	*/
	void
	destroy_object(
		Object::ID const object_id
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
