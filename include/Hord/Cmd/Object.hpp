/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief %Object commands.
@ingroup cmd_object
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/String.hpp>
#include <Hord/Data/Defs.hpp>
#include <Hord/Data/ValueRef.hpp>
#include <Hord/Data/Table.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Schema/Defs.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/Unit.hpp>

namespace Hord {
namespace Cmd {
namespace Object {

// Forward declarations
class Create;
class SetSlug;
class SetParent;

struct MetadataBase;
class SetMetaField;
class RenameMetaField;
class RemoveMetaField;

/**
	@addtogroup cmd_object
	@{
*/

/**
	Object create command.
*/
class Create final
	: public Cmd::Unit<Create>
{
	HORD_CMD_IMPL_BOILERPLATE_WITH_COMMIT(
		Create,
		"Cmd::Object::Create"
	);

public:
/** @name Operations */ /// @{
	/**
		Create object.

		If @a schema_id is non-null, it must refer to a @ref schema.

		If @a schema_ref is @c true and @a type is a @ref table,
		its @c schema_ref property will be set to @a schema_id.
	*/
	exec_result_type
	operator()(
		Hord::Object::Type type,
		Hord::Object::ID parent_id,
		Hord::Schema::ID schema_id,
		String const& slug,
		bool schema_ref
	) noexcept;
/// @}
};

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
	Set parent command.
*/
class SetParent final
	: public Cmd::Unit<SetParent>
{
	HORD_CMD_IMPL_BOILERPLATE(
		SetParent,
		"Cmd::Object::SetParent"
	);

public:
/** @name Operations */ /// @{
	/**
		Set parent.

		@param object Object to modify.
		@param new_parent New parent.
	*/
	exec_result_type
	operator()(
		Hord::Object::Unit& object,
		Hord::Object::ID const new_parent
	) noexcept;
/// @}
};

/**
	%Base metadata command properties.
*/
struct MetadataBase {
protected:
	signed m_field_index{-1};

public:
/** @name Properties */ /// @{
	/**
		Get field index.

		@returns -1 when no index was known.
	*/
	signed
	field_index() const noexcept {
		return m_field_index;
	}
/// @}
};

/**
	Set MetaField value command.
*/
class SetMetaField final
	: public Hord::Cmd::Object::MetadataBase
	, public Cmd::Unit<SetMetaField>
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
		bool const create = true
	) noexcept;
/// @}
};

/**
	Rename MetaField command.
*/
class RenameMetaField final
	: public Hord::Cmd::Object::MetadataBase
	, public Cmd::Unit<RenameMetaField>
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
	: public Hord::Cmd::Object::MetadataBase
	, public Cmd::Unit<RemoveMetaField>
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
HORD_CMD_IMPL_ENSURE_TRAITS(Cmd::Object::Create);
HORD_CMD_IMPL_ENSURE_TRAITS(Cmd::Object::SetSlug);
HORD_CMD_IMPL_ENSURE_TRAITS(Cmd::Object::SetParent);
HORD_CMD_IMPL_ENSURE_TRAITS(Cmd::Object::SetMetaField);
HORD_CMD_IMPL_ENSURE_TRAITS(Cmd::Object::RenameMetaField);
HORD_CMD_IMPL_ENSURE_TRAITS(Cmd::Object::RemoveMetaField);
/** @endcond */ // INTERNAL

} // namespace Cmd
} // namespace Hord
