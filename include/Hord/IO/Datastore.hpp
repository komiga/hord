/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Datastore class.
@ingroup datastore
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/traits.hpp>
#include <Hord/String.hpp>
#include <Hord/utility.hpp>
#include <Hord/System/IDGenerator.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/Prop.hpp>
#include <Hord/IO/StorageInfo.hpp>
#include <Hord/Object/Defs.hpp>

#include <duct/cc_unique_ptr.hpp>
#include <duct/StateStore.hpp>

#include <cassert>
#include <functional>
#include <iosfwd>

namespace Hord {

/** @cond INTERNAL */
namespace Cmd {
namespace Datastore {
	class Init;
} // namespace Datastore
} // namespace Cmd
/** @endcond */ // INTERNAL

namespace IO {

// Forward declarations
class Datastore;

/**
	@addtogroup datastore
	@{
*/

/**
	Base datastore.

	@note %Datastores must be locked when a single prop stream is
	active.
*/
class Datastore {
	friend class Cmd::Datastore::Init;

public:
	/**
		Owning pointer to datastore.
	*/
	using UPtr = duct::cc_unique_ptr<IO::Datastore>;

	/**
		Datastore ID.
	*/
	using ID = HashValue;

	enum : HashValue {
		/** Null ID. */
		ID_NULL = ID{HASH_EMPTY},
	};

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
		*/
		IO::Datastore::UPtr
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
	template<class D>
	struct ensure_traits
		: traits::require_t<
			D,
			tw::capture_post<std::is_base_of, Datastore>::type,
			std::is_nothrow_destructible
		>
		, traits::disallow_t<
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

	/** Object map. */
	using object_map_type
	= aux::unordered_map<
		Object::ID,
		Object::UPtr
	>;

	/** Object ID set. */
	using id_set_type
	= aux::unordered_set<
		Object::ID
	>;

protected:
/** @name Implementation */ /// @{
	/**
		open() implementation.

		@throws Error{ErrorCode::datastore_open_failed}
	*/
	virtual void
	open_impl(
		bool const create_if_nonexistent
	) = 0;

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
	virtual storage_info_map_type::const_iterator
	create_object_impl(
		Object::ID const object_id,
		Object::type_info const& type_info,
		IO::Linkage const linkage
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
		/** %Resident objects have been initialized. */
		initialized = 1 << 2,

		/** First reserved state. */
		RESERVED_FIRST = 1 << 3,
		/** Last reserved state. */
		RESERVED_LAST = 1 << 7
	};

/** @cond INTERNAL */
#define HORD_STATE_ASSERT_VALID_(x_)	\
	assert(								\
		State::RESERVED_FIRST > x_ ||	\
		State::RESERVED_LAST  < x_		\
	);
/** @endcond */

	/**
		Enable state.
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
	IO::Datastore::type_info const* m_type_info;
	IO::Datastore::ID m_id;
	duct::StateStore<State> m_states;
	String m_root_path;
	storage_info_map_type m_storage_info;
	object_map_type m_objects;
	id_set_type m_root_objects;

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
	*/
	Datastore(
		IO::Datastore::type_info const& tinfo,
		String root_path
	) noexcept;
/// @}

public:
/** @name Properties */ /// @{
	/**
		Get type info.
	*/
	IO::Datastore::type_info const&
	get_type_info() const noexcept {
		return *m_type_info;
	}

	/**
		Get ID.
	*/
	IO::Datastore::ID
	get_id() const noexcept {
		return m_id;
	}

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

	/**
		Check if the datastore is initialized.

		@note This state is assigned when Cmd::Datastore::Init has
		been executed.
	*/
	bool
	is_initialized() const noexcept {
		return test_state(State::initialized);
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

	/**
		Get resident object map (mutable).
	*/
	object_map_type&
	get_objects() noexcept {
		return m_objects;
	}

	/**
		Get resident object map.
	*/
	object_map_type const&
	get_objects() const noexcept {
		return m_objects;
	}

	/**
		Get resident root object set (mutable).
	*/
	id_set_type&
	get_root_objects() noexcept {
		return m_root_objects;
	}

	/**
		Get resident root object set.
	*/
	id_set_type const&
	get_root_objects() const noexcept {
		return m_root_objects;
	}
/// @}

/** @name Operations */ /// @{
	/**
		Open the datastore.

		@throws Error{ErrorCode::datastore_open_already}
		If the datastore is already open.

		@throws Error{ErrorCode::datastore_open_failed}
		If the datastore failed to open. [implementation]

		@param create_if_nonexistent Whether to create the datastore
		if it does not exist (implementation-defined).
	*/
	void
	open(
		bool const create_if_nonexistent
	);

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
		[implementation]

		@throws Error{ErrorCode::datastore_prop_void}
		If the object for @c prop_info.object_id has not yet created
		the prop (but it is otherwise valid). This can only occur
		when acquiring an input stream. [implementation]

		@throws Error{ErrorCode::datastore_prop_unsupplied}
		If the object for @c prop_info.object_id does not supply the
		requested prop. [implementation]

		@throws Error{..}
		<em>Implementation-defined exceptions.</em>

		@returns Raw prop stream.

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
		[implementation]

		@throws Error{..}
		<em>Implementation-defined exceptions.</em>

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
		Generate a unique ID.

		@throws Error{ErrorCode::datastore_closed}
		If the datastore is closed.
	*/
	Object::ID
	generate_id(
		System::IDGenerator& generator
	) const;

	/**
		Create an object in the datastore.

		@throws Error{ErrorCode::datastore_closed}
		If the datastore is closed.

		@throws Error{ErrorCode::datastore_locked}
		If the datastore is locked.

		@throws Error{ErrorCode::datastore_object_already_exists}
		If @a object_id already exists in the datastore.

		@throws Error{..}
		<em>Implementation-defined exceptions.</em>
	*/
	storage_info_map_type::const_iterator
	create_object(
		Object::ID const object_id,
		Object::type_info const& type_info,
		IO::Linkage const linkage
	);

	/**
		Destroy an object in the datastore.

		@throws Error{ErrorCode::datastore_closed}
		If the datastore is closed.

		@throws Error{ErrorCode::datastore_locked}
		If the datastore is locked.

		@throws Error{ErrorCode::datastore_object_not_found}
		If @a object_id does not exist in the datastore.
		[implementation]

		@throws Error{..}
		<em>Implementation-defined exceptions.</em>
	*/
	void
	destroy_object(
		Object::ID const object_id
	);

	/**
		Check if a resident object exists.
	*/
	bool
	has_object(
		Object::ID const id
	) const noexcept(noexcept(m_objects.find(id))) {
		return
			id.is_reserved()
			? false
			: m_objects.cend() != m_objects.find(id)
		;
	}

	/**
		Find resident object pointer by ID.

		@note Will return @c nullptr when <code>id == Object::ID_NULL</code>.
		@{
	*/
	Object::Unit*
	find_ptr(
		Object::ID const id
	) noexcept {
		if (Object::ID_NULL != id) {
			auto const it = m_objects.find(id);
			return
				m_objects.end() != it
				? it->second.get()
				: nullptr
			;
		} else {
			return nullptr;
		}
	}

	Object::Unit const*
	find_ptr(
		Object::ID const id
	) const noexcept {
		if (Object::ID_NULL != id) {
			auto const it = m_objects.find(id);
			return
				m_objects.cend() != it
				? it->second.get()
				: nullptr
			;
		} else {
			return nullptr;
		}
	}
	/** @} */

	/**
		Find resident object pointer by path.
		@{
	*/
	Object::Unit*
	find_ptr_path(
		String const& path
	) noexcept {
		return const_cast<Object::Unit*>(make_const(*this).find_ptr_path(path));
	}

	Object::Unit const*
	find_ptr_path(
		String const& path
	) const noexcept;
	/** @} */
/// @}
};

/** @} */ // end of doc-group datastore

} // namespace IO

template struct traits::require_t<
	IO::Datastore,
	std::has_virtual_destructor
>;

} // namespace Hord
