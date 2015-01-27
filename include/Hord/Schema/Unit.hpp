/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Schema unit class.
@ingroup schema
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/aux.hpp>
#include <Hord/serialization.hpp>
#include <Hord/Data/Metadata.hpp>
#include <Hord/IO/PropStream.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Object/Unit.hpp>
#include <Hord/Schema/Defs.hpp>

namespace Hord {
namespace Schema {

// Forward declarations
class Unit;

/**
	@addtogroup schema
	@{
*/

/**
	Base schema unit class.

	@note This object supplies the primary prop.
	Specialized units may supply the auxiliary prop.
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

private:
	Data::Metadata m_init_metadata{};

	Unit() = delete;
	Unit(Unit const&) = delete;
	Unit& operator=(Unit const&) = delete;

	void
	deserialize_prop_primary(
		IO::InputPropStream& prop_stream,
		InputSerializer& ser
	);
	void
	serialize_prop_primary(
		IO::OutputPropStream& prop_stream,
		OutputSerializer& ser
	) const;

// Object::Unit implementation
	void
	deserialize_impl(
		IO::InputPropStream& prop_stream
	) override;

	void
	serialize_impl(
		IO::OutputPropStream& prop_stream
	) const override;

protected:
	/**
		Deserialize auxiliary prop.
	*/
	virtual void
	deserialize_prop_auxiliary(
		IO::InputPropStream& prop_stream,
		InputSerializer& ser
	) = 0;

	/**
		Serialize auxiliary prop.
	*/
	virtual void
	serialize_prop_auxiliary(
		IO::OutputPropStream& prop_stream,
		OutputSerializer& ser
	) const = 0;

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
		Constructor with type information, ID, and parent.

		@post See Object::Unit.
	*/
	Unit(
		Object::type_info const& tinfo,
		Schema::ID const id,
		Object::ID const parent
	) noexcept;
/// @}

public:
/** @name Properties */ /// @{
	/**
		Get init metadata (mutable).
	*/
	Data::Metadata&
	get_init_metadata() noexcept {
		return m_init_metadata;
	}

	/**
		Get init metadata.
	*/
	Data::Metadata const&
	get_init_metadata() const noexcept {
		return m_init_metadata;
	}
/// @}
};

/** @} */ // end of doc-group schema

} // namespace Schema
} // namespace Hord
