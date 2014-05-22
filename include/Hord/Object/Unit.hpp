/**
@file Object/Unit.hpp
@brief Object base class.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef HORD_OBJECT_UNIT_HPP_
#define HORD_OBJECT_UNIT_HPP_

#include <Hord/config.hpp>
#include <Hord/aux.hpp>
#include <Hord/traits.hpp>
#include <Hord/String.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/PropStateStore.hpp>
#include <Hord/IO/PropStream.hpp>
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
	/**
		Ensure traits for deriving classes.

		@tparam D Deriving class.
	*/
	template<
		class D
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
	Object::type_info const* m_type_info;
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

/** @name Special member functions */ /// @{
public:
	/** Destructor. */
	virtual
	~Unit() noexcept = 0;

protected:
	/** Move constructor. */
	Unit(Unit&&);
	/** Move assignment operator. */
	Unit& operator=(Unit&&);

	/**
		Constructor with type info, %ID, and parent.

		@post
		@code
			get_storage_state()
			== (get_id() == Object::NULL_ID
				? IO::StorageState::null
				: IO::StorageState::placeholder
			)
		@endcode
	*/
	Unit(
		Object::type_info const& tinfo,
		Object::ID const id,
		Object::ID const parent
	) noexcept;
/// @}

public:
/** @name Properties */ /// @{
	/**
		Get type info.
	*/
	Object::type_info const&
	get_type_info() const noexcept {
		return *m_type_info;
	}

	/**
		Get type.
	*/
	Object::Type
	get_type() const noexcept {
		return m_type_info->type;
	}

	/**
		Get base type.
	*/
	Object::BaseType
	get_base_type() const noexcept {
		return get_type().base();
	}

	/**
		Get unit type.
	*/
	Object::UnitType
	get_unit_type() const noexcept {
		return get_type().unit();
	}

	/**
		Get unit name.
	*/
	char const*
	get_unit_name() const noexcept {
		return m_type_info->unit_name;
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
	*/
	void
	set_id(
		Object::ID const id
	) noexcept {
		m_id = id;
	}

	/**
		Get ID.

		@remarks This returns the in-hive ID, which yields
		Object::NULL_ID if the object is itself a hive.

		@sa get_id_bare()
	*/
	Object::ID
	get_id() const noexcept {
		return
			Object::BaseType::Hive == get_type().base()
			? Object::NULL_ID
			: get_id_bare()
		;
	}

	/**
		Get bare ID.

		@remarks This returns the bare ID without morphing it to
		Object::NULL_ID when the object is a hive.
	*/
	Object::ID
	get_id_bare() const noexcept {
		return m_id;
	}

	/**
		Check if object is null.

		@note An object is null iff its ID is Object::NULL_ID.

		@remarks Despite the hive being considered @c NULL_ID, at
		runtime it has a valid non-null ID outside of the ID space
		of its children.
	*/
	bool
	is_null() const noexcept {
		return Object::NULL_ID == m_id;
	}

	/**
		Get storage state.

		@sa IO::StorageState
	*/
	IO::StorageState
	get_storage_state() const noexcept {
		return
			  is_null()
				? IO::StorageState::null
			: m_prop_states.all_original()
				? IO::StorageState::original
			: m_prop_states.any_modified()
				? IO::StorageState::modified
			: !m_slug.empty()
				? IO::StorageState::placeholder_identified
			: IO::StorageState::placeholder
		;
	}

	/**
		Set parent.

		@warning This should not be used directly. See the
		referenced functions.

		@sa Object::unset_parent(),
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

		@warning This property is truncated to 255 code units.
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
		Set scratch space.
	*/
	void
	set_scratch_space(
		String scratch_space
	) noexcept {
		m_scratch_space.assign(std::move(scratch_space));
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

		@throws Error{ErrorCode::serialization_prop_unsupplied}
		If the object does not supply the prop that @a prop_stream
		represents.

		@throws Error{ErrorCode::serialization_io_failed}
		If an input operation failed.

		@throws Error{ErrorCode::serialization_data_malformed}
		If malformed data was encountered.
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

		@throws Error{ErrorCode::serialization_prop_unsupplied}
		If the object does not supply the prop that @a prop_stream
		represents.

		@throws Error{ErrorCode::serialization_prop_improper_state}
		If the state for the prop that @a prop_stream represents
		is uninitialized.

		@throws Error{ErrorCode::serialization_io_failed}
		If an output operation failed.
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
