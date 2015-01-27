/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <Hord/utility.hpp>
#include <Hord/Log.hpp>
#include <Hord/IO/Defs.hpp>
#include <Hord/IO/Prop.hpp>
#include <Hord/IO/Datastore.hpp>
#include <Hord/Object/Ops.hpp>
#include <Hord/Schema/Defs.hpp>
#include <Hord/Schema/UnitTable.hpp>
#include <Hord/Table/Defs.hpp>
#include <Hord/Table/Unit.hpp>
#include <Hord/Cmd/Table.hpp>
#include <Hord/System/Driver.hpp>
#include <Hord/System/Context.hpp>

#include <exception>

#include <Hord/detail/gr_core.hpp>

namespace Hord {
namespace Cmd {
namespace Table {

#define HORD_SCOPE_CLASS Cmd::Table::Create

#define HORD_SCOPE_FUNC commit_impl
HORD_CMD_IMPL_COMMIT_DEF(HORD_SCOPE_CLASS) {
	if (bad()) {
		m_object_id = Hord::Table::ID_NULL;
	}
}
#undef HORD_SCOPE_FUNC

#define HORD_SCOPE_FUNC exec // pseudo
HORD_SCOPE_CLASS::exec_result_type
HORD_SCOPE_CLASS::operator()(
	Hord::Object::ID const parent_id,
	Hord::Schema::ID const schema_id,
	String const& slug,
	Hord::Table::UnitType const unit_type,
	bool const attach_to_schema
) noexcept try {
	auto& driver = get_driver();
	auto& datastore = get_datastore();

	// Validate
	auto const* schema_obj = datastore.find_ptr(schema_id);
	if (
		parent_id != Hord::Object::ID_NULL &&
		!datastore.has_object(parent_id)
	) {
		return commit_error("parent not found");
	} else if (
		Hord::Schema::ID_NULL != schema_id &&
		!schema_obj
	) {
		return commit_error("schema not found");
	} else if (
		Hord::Schema::ID_NULL != schema_id &&
		Hord::Object::BaseType::Schema != schema_obj->get_base_type()
	) {
		return commit_error("given schema ID is not a schema");
	} else if (slug.empty()) {
		return commit_error("slug empty");
	} else if (Hord::Object::SLUG_MAX_SIZE < slug.size()) {
		return commit_error("slug too long");
	} else {
		// TODO: Only check children of parent
		// (all should be resident when command is executed)
		for (auto const& pair : datastore.get_objects()) {
			auto const* object = pair.second.get();
			if (
				nullptr != object &&
				parent_id  == object->get_parent() &&
				slug    == object->get_slug()
			) {
				return commit_error("slug already exists");
			}
		}
	}

	// Create
	auto const* tinfo = driver.get_object_type_info(
		Hord::Table::Type{unit_type}
	);
	if (nullptr == tinfo) {
		return commit_error("unknown unit type");
	}
	// NB: Leak ErrorCode::datastore_closed
	m_object_id = datastore.generate_id(driver.get_id_generator());
	auto obj = tinfo->construct(m_object_id, Hord::Object::ID_NULL);
	if (nullptr == obj) {
		return commit_error("allocation failed");
	}
	auto emplace_pair = datastore.get_objects().emplace(m_object_id, std::move(obj));
	if (!emplace_pair.second) {
		return commit_error("id already exists");
	}
	datastore.create_object(
		m_object_id, *tinfo, IO::Linkage::resident
	);

	// Properties
	auto& table = static_cast<Hord::Table::Unit&>(
		*emplace_pair.first->second
	);
	Hord::Object::set_parent(table, datastore, parent_id);
	table.set_slug(slug);
	if (attach_to_schema) {
		table.set_schema_ref(schema_id);
	}
	if (schema_obj) {
		auto schema_obj_typed = static_cast<Hord::Schema::Unit const*>(schema_obj);
		table.get_metadata().table().assign(schema_obj_typed->get_init_metadata().table());
		if (Hord::Schema::UnitType::Table == static_cast<Hord::Schema::UnitType>(schema_obj->get_unit_type())) {
			table.get_data().replace_schema(
				static_cast<Hord::Schema::UnitTable const*>(schema_obj)->get_data_schema()
			);
		}
	}
	table.get_prop_states().assign_all(IO::PropState::modified);

	return commit();
} catch (Error const& err) {
	notify_exception_current();
	if (ErrorCode::datastore_object_already_exists == err.get_code()) {
		return commit_error("id already exists");
	}
	return commit_error("unknown error");
} catch (...) {
	notify_exception_current();
	return commit_error("unknown error");
}
#undef HORD_SCOPE_FUNC

#undef HORD_SCOPE_CLASS

} // namespace Table
} // namespace Cmd
} // namespace Hord
