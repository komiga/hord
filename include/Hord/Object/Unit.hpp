/**
@file Object/Unit.hpp
@brief Object base class.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_OBJECT_UNIT_HPP_
#define HORD_OBJECT_UNIT_HPP_

#include <Hord/config.hpp>
#include <Hord/aux.hpp>
#include <Hord/traits.hpp>
#include <Hord/String.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/Prop.hpp>
#include <Hord/Data/Metadata.hpp>
#include <Hord/Object/Defs.hpp>

namespace Hord {
namespace Object {

// Forward declarations
class Unit;

/**
	@addtogroup object
	@{
*/

/**
	Base object.

	@sa Hive::Unit,
		Rule::Unit,
		Node::Unit
*/
class Unit {
public:
	/** Type info. */
	using type_info = Object::type_info;

	/**
		Ensure traits for deriving classes.

		@tparam D Deriving class.
	*/
	template<
		typename D
	>
	struct ensure_traits :
		traits::require_t<
			D,
			tw::capture_post<std::is_base_of, Object::Unit>::type,
			std::is_nothrow_destructible,
			tw::is_fully_moveable
		>,
		traits::disallow_t<
			D,
			tw::is_copyable
		>
	{};

	/** ID set. */
	using id_set_type
	= aux::unordered_set<
		Object::ID
	>;

private:
	Object::ID m_id;
	IO::PropStateStore m_prop_states;
	id_set_type m_children{}; // runtime

	Object::ID m_parent;
	String m_slug{};
	Data::Metadata m_metadata{};
	String m_scratch_space{};

	Unit() = delete;
	Unit(Unit const&) = delete;
	Unit& operator=(Unit const&) = delete;

protected:
/** @name Implementation */ /// @{
	/**
		get_type_info() implementation.
	*/
	virtual type_info const&
	get_type_info_impl() const noexcept = 0;

	/**
		deserialize() implementation.

		@note This is only called for the primary and auxiliary
		props.

		@note Implementation must ensure state is retained if it
		throws an exception. Object::Unit will not throw an
		exception after a call to this function.

		@throws Error{ErrorCode::serialization_io_failed}
		@throws Error{ErrorCode::serialization_data_malformed}
	*/
	virtual void
	deserialize_impl(
		IO::InputPropStream& prop_stream
	) = 0;

	/**
		serialize() implementation.

		@note This is only called for the primary and auxiliary
		props.

		@note Implementation must ensure state is retained if it
		throws an exception. Object::Unit will not throw an
		exception after a call to this function.

		@throws Error{ErrorCode::serialization_io_failed}
	*/
	virtual void
	serialize_impl(
		IO::OutputPropStream& prop_stream
	) const = 0;
/// @}

protected:
/** @name Constructors and destructor */ /// @{
	/**
		Constructor with supplied props, %ID, and parent.

		@post
		@code
			get_storage_state()
			== (get_id() == Object::NULL_ID
				? IO::StorageState::null
				: IO::StorageState::placeholder
			)
		@endcode

		@param supplies_primary Whether IO::PropType::primary is
		supplied by the object.
		@param supplies_auxiliary Whether IO::PropType::auxiliary is
		supplied by the object.
		@param id Object %ID.
		@param parent Parent %ID.

		@sa IO::PropStateStore
	*/
	Unit(
		bool const supplies_primary,
		bool const supplies_auxiliary,
		Object::ID const id,
		Object::ID const parent
	) noexcept;

	/** Move constructor. */
	Unit(Unit&&);
public:
	/** Destructor. */
	virtual
	~Unit() noexcept = 0;
/// @}

protected:
/** @name Operators */ /// @{
	/** Move assignment operator. */
	Unit& operator=(Unit&&);
/// @}

public:
/** @name Properties */ /// @{
	/**
		Get type info.
	*/
	type_info const&
	get_type_info() const noexcept {
		return get_type_info_impl();
	}

	/**
		Get type.
	*/
	Object::Type
	get_type() const noexcept {
		return get_type_info().type;
	}

	/**
		Get storage state.
	*/
	IO::StorageState
	get_storage_state() const noexcept {
		return
			  Object::NULL_ID == m_id
				? IO::StorageState::null
			: m_prop_states.all_original()
				? IO::StorageState::original
			: m_prop_states.any_modified()
				? IO::StorageState::modified
			: IO::StorageState::placeholder
		;
	}

	/**
		Get prop state store.
	*/
	IO::PropStateStore const&
	get_prop_states() const noexcept {
		return m_prop_states;
	}

	/**
		Get mutable prop state store.
	*/
	IO::PropStateStore&
	get_prop_states() noexcept {
		return m_prop_states;
	}

	/**
		Get children set.
	*/
	id_set_type const&
	get_children() const noexcept {
		return m_children;
	}

	/**
		Get mutable children set.
	*/
	id_set_type&
	get_children() noexcept {
		return m_children;
	}

	/**
		Set ID.

		@param id New ID.
	*/
	void
	set_id(
		Object::ID const id
	) noexcept {
		m_id = id;
	}

	/**
		Get ID.
	*/
	Object::ID
	get_id() const noexcept {
		return m_id;
	}

	/**
		Check if object is identified.

		@note An object is identified iff its ID is non-null.
	*/
	bool
	is_identified() const noexcept {
		return Object::NULL_ID != m_id;
	}

	/**
		Set parent.

		@warning This should not be used directly. See the
		referenced functions.

		@param parent New parent.

		@sa Object::unset_parent()
			Object::set_parent()
	*/
	void
	set_parent(
		Object::ID const parent
	) noexcept {
		m_parent = parent;
	}

	/**
		Get parent.
	*/
	Object::ID
	get_parent() const noexcept {
		return m_parent;
	}

	/**
		Set slug.

		@warning This property is truncated to 64 code units.

		@param slug New slug.
	*/
	void
	set_slug(
		String slug
	) noexcept;

	/**
		Get slug.
	*/
	String const&
	get_slug() const noexcept {
		return m_slug;
	}

	/**
		Get metadata.
	*/
	Data::Metadata const&
	get_metadata() const noexcept {
		return m_metadata;
	}

	/**
		Get mutable metadata.
	*/
	Data::Metadata&
	get_metadata() noexcept {
		return m_metadata;
	}

	/**
		Get scratch space.
	*/
	String const&
	get_scratch_space() const noexcept {
		return m_scratch_space;
	}

	/**
		Get mutable scratch space.
	*/
	String&
	get_scratch_space() noexcept {
		return m_scratch_space;
	}
/// @}

/** @name Serialization */ /// @{
	/**
		Deserialize prop.

		@note State will be retained if an exception is thrown.

		@post With no exceptions:
		@code
			get_prop_states().has(
				prop_stream.get_type(),
				IO::PropState::original
			)
		@endcode

		@throws Error{ErrorCode::serialization_prop_unsupplied}
		If the object does not supply the prop that @a prop_stream
		represents.

		@throws Error{ErrorCode::serialization_io_failed}
		If an input operation failed.

		@throws Error{ErrorCode::serialization_data_malformed}
		If malformed data was encountered.

		@param prop_stream Prop stream.
	*/
	void
	deserialize(
		IO::InputPropStream& prop_stream
	);

	/**
		Serialize prop.

		@note State will be retained if an exception is thrown.

		@warning If ErrorCode::serialization_io_failed is thrown,
		the prop stream likely contains malformed data.

		@post With no exceptions:
		@code
			get_prop_states().has(
				prop_stream.get_type(),
				IO::PropState::original
			)
		@endcode

		@throws Error{ErrorCode::serialization_prop_unsupplied}
		If the object does not supply the prop that @a prop_stream
		represents.

		@throws Error{ErrorCode::serialization_prop_improper_state}
		If the state for the prop that @a prop_stream represents
		is uninitialized.

		@throws Error{ErrorCode::serialization_io_failed}
		If an output operation failed.

		@param prop_stream Prop stream.
	*/
	void
	serialize(
		IO::OutputPropStream& prop_stream
	);
/// @}
};

/** @} */ // end of doc-group object

} // namespace Object

template struct traits::require_t<
	Object::Unit,
	std::has_virtual_destructor
>;

} // namespace Hord

#endif // HORD_OBJECT_UNIT_HPP_
