/**
@file Hive/Unit.hpp
@brief Base hive unit class.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/aux.hpp>
#include <Hord/IO/PropStream.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Object/Unit.hpp>
#include <Hord/Hive/Defs.hpp>

namespace Hord {
namespace Hive {

// Forward declarations
class Unit;

/**
	@addtogroup object
	@{
*/
/**
	@addtogroup hive
	@{
*/

/**
	Base hive unit class.

	@note This object supplies no data props. Specialized units
	shall not change serialization.
*/
class Unit
	: public Object::Unit
{
private:
	using base = Object::Unit;

public:
	/**
		Ensure traits for deriving classes.

		@tparam D Deriving class.
	*/
	template<class D>
	struct ensure_traits
		: base::ensure_traits<D>
	{};

	/** Object map. */
	using object_map_type
	= aux::unordered_map<
		Object::ID,
		Object::UPtr
	>;

private:
	object_map_type m_objects{};

	Unit() = delete;
	Unit(Unit const&) = delete;
	Unit& operator=(Unit const&) = delete;

	// Object::Unit implementation

	void
	deserialize_impl(
		IO::InputPropStream& prop_stream
	) override;

	void
	serialize_impl(
		IO::OutputPropStream& prop_stream
	) const override;

public:
/** @name Special member functions */ /// @{
	/** Destructor. */
	virtual
	~Unit() noexcept override = 0;

	/** Move constructor. */
	Unit(Unit&&);
	/** Move assignment operator. */
	Unit& operator=(Unit&&);

protected:
	/**
		Constructor with type information and ID.

		@post See Object::Unit.
	*/
	explicit
	Unit(
		Object::type_info const& tinfo,
		Hive::ID const id
	) noexcept;
/// @}

public:
/** @name Properties */ /// @{
	/**
		Get object map.
	*/
	object_map_type const&
	get_objects() const noexcept {
		return m_objects;
	}

	/**
		Get mutable object map.
	*/
	object_map_type&
	get_objects() noexcept {
		return m_objects;
	}
/// @}

/** @name Objects */ /// @{
	/**
		Check if an object exists.
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
		Find object pointer by ID.

		@note Will return @c this when <code>id == Object::ID_HIVE</code>
		and @c nullptr when <code>id == Object::ID_NULL</code>.
	*/
	Object::Unit*
	find_ptr(
		Object::ID const id
	) noexcept {
		if (Object::ID_NULL == id) {
			return nullptr;
		} else if (Object::ID_HIVE == id) {
			return this;
		} else {
			auto const it = m_objects.find(id);
			return
				m_objects.end() != it
				? it->second.get()
				: nullptr
			;
		}
	}

	/** @copydoc find_ptr(Object::ID const id) noexcept */
	Object::Unit const*
	find_ptr(
		Object::ID const id
	) const noexcept {
		if (Object::ID_NULL == id) {
			return nullptr;
		} else if (Object::ID_HIVE == id) {
			return this;
		} else {
			auto const it = m_objects.find(id);
			return
				m_objects.cend() != it
				? it->second.get()
				: nullptr
			;
		}
	}
/// @}
};

/** @} */ // end of doc-group hive
/** @} */ // end of doc-group object

} // namespace Hive
} // namespace Hord
