/**
@file
@brief %Object commands.
@ingroup cmd_object

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/String.hpp>
#include <Hord/Data/Defs.hpp>
#include <Hord/Data/Table.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/Unit.hpp>

namespace Hord {
namespace Cmd {
namespace Object {

// Forward declarations
class SetSlug;
class SetMetaField;
class RenameMetaField;
class RemoveMetaField;

/**
	@addtogroup cmd_object
	@{
*/

/**
	Set slug command.
*/
class SetSlug final
	: public Cmd::Unit<SetSlug>
{
	HORD_CMD_IMPL_BOILERPLATE(
		SetSlug,
		"Cmd::Object::SetSlug"
	);

public:
/** @name Operations */ /// @{
	/**
		Set slug.

		@param object Object to modify.
		@param new_slug New slug.
	*/
	exec_result_type
	operator()(
		Hord::Object::Unit& object,
		String new_slug
	) noexcept;
/// @}
};

/**
	Set MetaField value command.
*/
class SetMetaField final
	: public Cmd::Unit<SetMetaField>
{
	HORD_CMD_IMPL_BOILERPLATE(
		SetMetaField,
		"Cmd::Object::SetMetaField"
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
		Hord::Object::Unit& object,
		unsigned const index,
		Data::ValueRef const& new_value
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
		Hord::Object::Unit& object,
		String const& name,
		Data::ValueRef const& new_value,
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
		"Cmd::Object::RenameMetaField"
	);

private:
	Cmd::Result
	set_name(
		Hord::Object::Unit& object,
		Hord::Data::Table::Iterator& it,
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
		Hord::Object::Unit& object,
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
		Hord::Object::Unit& object,
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
		"Cmd::Object::RemoveMetaField"
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
		Hord::Object::Unit& object,
		unsigned const index
	) noexcept;

	/**
		Remove field by name.

		@param object Object to modify.
		@param name Name of field.
	*/
	exec_result_type
	operator()(
		Hord::Object::Unit& object,
		String const& name
	) noexcept;
/// @}
};

/** @} */ // end of doc-group cmd_object

} // namespace Object

/** @cond INTERNAL */
HORD_CMD_IMPL_ENSURE_TRAITS(Cmd::Object::SetMetaField);
HORD_CMD_IMPL_ENSURE_TRAITS(Cmd::Object::RenameMetaField);
HORD_CMD_IMPL_ENSURE_TRAITS(Cmd::Object::RemoveMetaField);
/** @endcond */ // INTERNAL

} // namespace Cmd
} // namespace Hord
