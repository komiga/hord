/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/String.hpp>
#include <Hord/Object/Defs.hpp>
#include <Hord/Schema/UnitTable.hpp>

#include <Hord/detail/gr_ceformat.hpp>

namespace Hord {
namespace Schema {

// class UnitTable implementation

#define HORD_SCOPE_CLASS Schema::UnitTable

constexpr Object::TypeInfo const
Schema::UnitTable::info;

Object::UPtr
UnitTable::construct(
	Object::ID const id,
	Object::ID const parent
) noexcept {
	return Object::UPtr{
		new(std::nothrow) Schema::UnitTable(Schema::ID{id}, parent)
	};
}

UnitTable::~UnitTable() noexcept = default;

UnitTable::UnitTable(UnitTable&&) = default;
UnitTable& UnitTable::operator=(UnitTable&&) = default;

UnitTable::UnitTable(
	Schema::ID const id,
	Object::ID const parent
) noexcept
	: base(
		Schema::UnitTable::info,
		id,
		parent
	)
{}

void
UnitTable::deserialize_prop_auxiliary(
	IO::InputPropStream& /*prop_stream*/,
	InputSerializer& ser
) {
	Data::TableSchema des_data_schema;
	ser(des_data_schema);

	// commit
	m_data_schema = std::move(des_data_schema);
}

void
UnitTable::serialize_prop_auxiliary(
	IO::OutputPropStream& /*prop_stream*/,
	OutputSerializer& ser
) const {
	ser(m_data_schema);
}

#undef HORD_SCOPE_CLASS

} // namespace Schema
} // namespace Hord
