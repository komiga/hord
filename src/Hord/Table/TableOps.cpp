/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/IO/Datastore.hpp>
#include <Hord/Schema/Defs.hpp>
#include <Hord/Schema/UnitTable.hpp>
#include <Hord/Table/Unit.hpp>

namespace Hord {
namespace Table {

constexpr static Hord::Schema::Type const
TYPE_SCHEMA_TABLE{Hord::Schema::UnitType::Table};

bool
set_schema_ref(
	Table::Unit& table,
	IO::Datastore& datastore,
	Schema::ID new_schema_ref
) {
	if (table.schema_ref() == new_schema_ref) {
		return false;
	}

	auto ref_obj = datastore.find_ptr(table.schema_ref());
	if (ref_obj && ref_obj->type() == TYPE_SCHEMA_TABLE) {
		auto schema_obj = static_cast<Schema::UnitTable*>(ref_obj);
		auto& referrers = schema_obj->referrers();
		auto it = referrers.find(table.id());
		if (it != referrers.cend()) {
			referrers.erase(it);
		}
	}
	table.set_schema_ref(Schema::ID_NULL);

	ref_obj = datastore.find_ptr(new_schema_ref);
	if (!new_schema_ref.is_null() && !ref_obj) {
		return false;
	} else if (ref_obj && ref_obj->type() == TYPE_SCHEMA_TABLE) {
		auto schema_obj = static_cast<Schema::UnitTable*>(ref_obj);
		schema_obj->referrers().emplace(table.id());
		table.set_schema_ref(new_schema_ref);
	}
	return true;
}

} // namespace Table
} // namespace Hord
