/**
@file Cmd/Data.hpp
@brief %Data commands.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <Hord/config.hpp>
#include <Hord/Data/Defs.hpp>
#include <Hord/Cmd/Defs.hpp>
#include <Hord/Cmd/Unit.hpp>

namespace Hord {
namespace Cmd {
namespace Data {

// Forward declarations
class SetMetaField;

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
	)

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
		Set field by index.

		@param index Index of field to change.
		@param new_value New value for field.
	*/
	result_type
	operator()(
		Object::Unit& object,
		unsigned const index,
		Hord::Data::FieldValue const& new_value
	) noexcept;

	/**
		Set field by name.

		@param name Name of field to change.
		@param new_value New value for field.
		@param create Whether to create the field if it does not exist.
	*/
	result_type
	operator()(
		Object::Unit& object,
		String const& name,
		Hord::Data::FieldValue const& new_value,
		bool const create = false
	) noexcept;
/// @}
};

/** @} */ // end of doc-group cmd_data
/** @} */ // end of doc-group cmd

} // namespace Data

HORD_CMD_IMPL_ENSURE_TRAITS(Cmd::Data::SetMetaField);

} // namespace Cmd
} // namespace Hord
