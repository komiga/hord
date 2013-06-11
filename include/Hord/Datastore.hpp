/**
@file Datastore.hpp
@brief Datastore class.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_DATASTORE_HPP_
#define HORD_DATASTORE_HPP_

#include "./config.hpp"
#include "./traits.hpp"
#include "./common_types.hpp"
#include "./common_enums.hpp"
#include "./Prop.hpp"
#include "./Hive.hpp"

#include <functional>
#include <iosfwd>

namespace Hord {

// Forward declarations
class Datastore;

/**
	@addtogroup serialization
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

	@remarks The datastore's hive should not be used by
	an implementation to lookup objects within the datastore. The
	hive is not guaranteed to contain the entire idset at runtime.

	@remarks The hive will always have a valid ID, but
	note that the hive's ID is <strong>not</strong> within the ID
	space of its children -- see @ref hive.
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
			- The constructed datastore;
			- or @c nullptr if construction failed.
			@param root_path Root path.
			@param id %Hive ID.
		*/
		Datastore*
		(&construct)(
			String root_path,
			HiveID const id
		) noexcept;
	};

	/**
		Ensure traits for deriving classes.

		@remarks All constructors should be hidden or deleted.

		@tparam D Deriving class.
	*/
	template<typename D>
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
			tw::capture<std::is_constructible, String, HiveID const>::type,
			tw::is_fully_copyable,
			tw::is_fully_moveable
		>
	{};

private:
	unsigned m_states;
	String m_root_path;
	Hive m_hive;

	Datastore()=delete;
	Datastore(Datastore const&)=delete;
	Datastore(Datastore&&)=delete;
	Datastore& operator=(Datastore const&)=delete;
	Datastore& operator=(Datastore&&)=delete;

protected:
/** @name Implementation */ /// @{
	/**
		open() implementation.
	*/
	virtual void
	open_impl()=0;
	/**
		close() implementation.

		@remarks This is not called if @c is_open()==false.
	*/
	virtual void
	close_impl()=0;

	/**
		acquire_input_stream() implementation.
	*/
	virtual std::istream&
	acquire_input_stream_impl(
		PropInfo const& prop_info
	)=0;
	/**
		acquire_output_stream() implementation.
	*/
	virtual std::ostream&
	acquire_output_stream_impl(
		PropInfo const& prop_info
	)=0;

	/**
		release_input_stream() implementation.
	*/
	virtual void
	release_input_stream_impl(
		PropInfo const& prop_info
	)=0;
	/**
		release_output_stream() implementation.
	*/
	virtual void
	release_output_stream_impl(
		PropInfo const& prop_info
	)=0;
/// @}

/** @name Internal state */ /// @{
	/**
		States.

		Implementations are permitted to define states @c 1<<8
		to @c 1<<31.
	*/
	enum class State : uint32_t {
		/** %Datastore is open. */
		opened	=1<<0,
		/** %Datastore is locked. */
		locked	=1<<1,

		/** First reserved state. */
		RESERVED_FIRST=1<<2,
		/** Last reserved state. */
		RESERVED_LAST=1<<7
	};

	/**
		Enable state.
		@param state %State to enable.
	*/
	void
	enable_state(
		State const state
	) noexcept;
	/**
		Disable state.
		@param state %State to disable.
	*/
	void
	disable_state(
		State const state
	) noexcept;
	/**
		Check if a state is enabled.

		@returns
		- @c true if the state is enabled;
		- @c false if the state is disabled.
		@param state %State to test.
	*/
	bool
	has_state(
		State const state
	) const noexcept;
/// @}

/** @name Constructors and destructor */ /// @{
	/**
		Constructor with root path and hive ID.

		@param root_path Root path.
		@param id %Hive ID.
	*/
	Datastore(
		String root_path,
		HiveID const id
	) noexcept;
public:
	/** Destructor. */
	virtual
	~Datastore()=0;
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
		Get hive.
		@returns %Hive.
	*/
	Hive const&
	get_hive() const noexcept {
		return m_hive;
	}

	/**
		Get mutable hive.
		@returns Mutable hive.
	*/
	Hive&
	get_hive() noexcept {
		return m_hive;
	}

	/**
		Check if the datastore is open.
		@returns
		- @c true if the datastore is open;
		- @c false if it is closed.
	*/
	bool
	is_open() const noexcept {
		return has_state(State::opened);
	}

	/**
		Check if the datastore is locked.
		@returns
		- @c true if the datastore is locked;
		- @c false if it is not locked.
	*/
	bool
	is_locked() const noexcept {
		return has_state(State::locked);
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

		@sa PropInfo,
			InputPropStream,
			OutputPropStream
	*/
	std::istream&
	acquire_input_stream(
		PropInfo const& prop_info
	);
	/** @copydoc acquire_input_stream(PropInfo const&) */
	std::ostream&
	acquire_output_stream(
		PropInfo const& prop_info
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

		@sa PropInfo,
			InputPropStream,
			OutputPropStream
	*/
	void
	release_input_stream(
		PropInfo const& prop_info
	);
	/** @copydoc release_input_stream(PropInfo const&) */
	void
	release_output_stream(
		PropInfo const& prop_info
	);
/// @}
};

template struct traits::require_t<
	Datastore,
	std::has_virtual_destructor
>;

/** @} */ // end of doc-group datastore
/** @} */ // end of doc-group serialization

} // namespace Hord

#endif // HORD_DATASTORE_HPP_
