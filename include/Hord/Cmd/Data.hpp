/**
@file Cmd/Data.hpp
@brief %Data commands.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/String.hpp>
#include <Hord/Data/Defs.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/Unit.hpp>

namespace Hord {
namespace Cmd {
namespace Data {

// Forward declarations
class SetMetaField;
class RenameMetaField;
class RemoveMetaField;

/**
	@addtogroup cmd
	@{
*/
/**
	@addtogroup cmd_data
	@{
*/

/**
	Set MetaField value command.
*/
class SetMetaField final
	: public Cmd::Unit<SetMetaField>
{
	HORD_CMD_IMPL_BOILERPLATE(
		SetMetaField,
		"Cmd::Data::SetMetaField"
	);

private:
	bool m_created{false};

public:
/** @name Properties */ /// @{
	/**
		Get whether a new field was created.
	*/
	bool created() const noexcept {
		return m_created;
	}
/// @}

/** @name Operations */ /// @{
	/**
		Set field value by index.

		@param object Object to modify.
		@param index Index of field.
		@param new_value New value for field.
	*/
	exec_result_type
	operator()(
		Object::Unit& object,
		unsigned const index,
		Hord::Data::FieldValue const& new_value
	) noexcept;

	/**
		Set field value by name.

		@param object Object to modify.
		@param name Name of field.
		@param new_value New value for field.
		@param create Whether to create the field if it does not exist.
	*/
	exec_result_type
	operator()(
		Object::Unit& object,
		String const& name,
		Hord::Data::FieldValue const& new_value,
		bool const create = false
	) noexcept;
/// @}
};

/**
	Rename MetaField command.
*/
class RenameMetaField final
	: public Cmd::Unit<RenameMetaField>
{
	HORD_CMD_IMPL_BOILERPLATE(
		RenameMetaField,
		"Cmd::Data::RenameMetaField"
	);

private:
	Cmd::Result
	set_name(
		Object::Unit& object,
		Hord::Data::MetaField& field,
		String const& new_name
	);

public:
/** @name Operations */ /// @{
	/**
		Rename field by index.

		@param object Object to modify.
		@param index Index of field.
		@param new_name New name.
	*/
	exec_result_type
	operator()(
		Object::Unit& object,
		unsigned const index,
		String const& new_name
	) noexcept;

	/**
		Rename field by name.

		@param object Object to modify.
		@param old_name Name of field.
		@param new_name New name.
	*/
	exec_result_type
	operator()(
		Object::Unit& object,
		String const& old_name,
		String const& new_name
	) noexcept;
/// @}
};

/**
	Rename MetaField command.
*/
class RemoveMetaField final
	: public Cmd::Unit<RemoveMetaField>
{
	HORD_CMD_IMPL_BOILERPLATE(
		RemoveMetaField,
		"Cmd::Data::RemoveMetaField"
	);

public:
/** @name Operations */ /// @{
	/**
		Remove field by index.

		@param object Object to modify.
		@param index Index of field.
	*/
	exec_result_type
	operator()(
		Object::Unit& object,
		unsigned const index
	) noexcept;

	/**
		Remove field by name.

		@param object Object to modify.
		@param name Name of field.
	*/
	exec_result_type
	operator()(
		Object::Unit& object,
		String const& name
	) noexcept;
/// @}
};

/** @} */ // end of doc-group cmd_data
/** @} */ // end of doc-group cmd

} // namespace Data

/** @cond INTERNAL */
HORD_CMD_IMPL_ENSURE_TRAITS(Cmd::Data::SetMetaField);
HORD_CMD_IMPL_ENSURE_TRAITS(Cmd::Data::RenameMetaField);
HORD_CMD_IMPL_ENSURE_TRAITS(Cmd::Data::RemoveMetaField);
/** @endcond */ // INTERNAL

} // namespace Cmd
} // namespace Hord
